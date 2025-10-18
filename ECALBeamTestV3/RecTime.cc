#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "TString.h"
#include "data_model.hh"
#include "RecHit.hh"
#include "Hit2Cluster.hh"
#include "Cluster.hh"
#include "Shower.hh"
#include "Neighbor.hh"
#include "Parameter.hh"
#include "SeedFinder.hh"
#include "Cluster2Shower.hh"
#include <TFile.h>
#include <TProfile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TVector3.h>
#include <map>
#include <vector>
#include "TChain.h"

#include <eigen3/Eigen/Dense>
#include <assert.h>

using namespace std;
using namespace TMath;
using namespace Eigen;

Double_t ff(double *x,double *par){
  Double_t val=par[0]*exp(-(x[0]-par[1])/par[2])+par[3]*exp(-(x[0]-par[1])/par[4])+par[5]*exp(-(x[0]-par[1])/par[6])+par[7]*exp(-(x[0]-par[1])/par[8])+par[9]*exp(-(x[0]-par[1])/par[10]);

  if(x[0]>=par[1]&&x[0]<=2000)
    return val*Power((x[0]-par[1]),2);
  else return 0;
}

bool isTextFile(const string& fileName){
  size_t dotPos = fileName.find_last_of('.');
  if(dotPos == std::string::npos) return false;

  std::string extension = fileName.substr(dotPos + 1);
  if(extension == "txt")
    return true;
  else 
    return false;
}

bool OnePulseFit(int cc, VectorXd timingvec, double* time, double* amp, double* pedestal, double* chi2, TF1* f1);

int main(int argc, char const *argv[]){
  /*TString filename;
  if(argc>=2)
    filename = argv[1];
  else cout<<"Missing filename"<<endl;

  TFile* file = TFile::Open(filename.Data());
  if(!file||file->IsZombie()){
    cout<<" Error in opening file"<<endl;
    return 1;
  }*/

  vector<string> datafiles;

  if(isTextFile(argv[1])){
    ifstream filestream(argv[1]);
    if(filestream.is_open()){
      string filename;
      while(getline(filestream,filename)) datafiles.push_back(filename);
    }
    else cout<< "error in reading filelist  "<<endl;
  }
  else datafiles.push_back(argv[1]);

  TChain *tree = new TChain("decode_data");
  
  for(size_t i=0;i<datafiles.size();i++) tree->AddFile(datafiles.at(i).data());

  //TTree* tree = (TTree*)file->Get("decode_data");
  Long64_t EventID;
  int TriggerID;
  Long64_t TimeCode;
  tree->SetBranchAddress("EventID",&EventID);
  tree->SetBranchAddress("TriggerID",&TriggerID);
  tree->SetBranchAddress("TimeCode",&TimeCode);
  vector<decode_data_col*> Hit(25);
  string Name[25];
  for(int i=0;i<25;i++){
    int no;
    if(i%5==0) no=3; 
    if(i%5==1) no=4; 
    if(i%5==2) no=5; 
    if(i%5==3) no=1; 
    if(i%5==4) no=2; 
    Name[i]=to_string(i/5+1)+"_"+to_string(no);
  }
  for(int i=0;i<25;i++) Hit[i]= new decode_data_col();
  for(int i=0;i<25;i++){
    string name="Hit_"+Name[i];
    tree->SetBranchAddress(name.data(),Hit[i]);
  }

  TString outputfile;
  if(argc==3)
    outputfile = argv[2];
  else{  
    outputfile = "RecTime.root";
    cout << "Auto save file as RecTime.root..." << endl;
  }

  TFile *f = new TFile(outputfile.Data(),"recreate");

  Long64_t eventID;
  int triggerID;
  Long64_t timecode;

  vector<int> HitID;
  vector<double> HitEnergy;
  vector<double> HitTime;

  TTree *rec_data = new TTree("rec_data","rec_data");
  rec_data->Branch("eventid",&eventID,"eventID/L");
  rec_data->Branch("triggerID",&triggerID,"triggerID/I");
  rec_data->Branch("timecode",&timecode,"timecode/L");
  rec_data->Branch("HitID",&HitID);
  rec_data->Branch("HitEnergy",&HitEnergy);
  rec_data->Branch("HitTime",&HitTime);

  Parameter& Para=Parameter::GetInstance();

  TF1* f1 = new TF1("f",ff,0,2000,11);

  /*for(int i=0;i<25;i++){
    for(int j=0;j<11;j++) cout<<Para.LGWfPara(i,j)<<" "; cout<<endl;
    for(int j=0;j<11;j++) cout<<Para.HGWfPara(i,j)<<" "; cout<<endl;
    }*/

  //int nEntries=tree->GetEntries()/10;
  int nEntries=tree->GetEntries();
  int interval = nEntries/20;

  for(int i=0;i<nEntries;i++){
  //for(int i=0;i<2;i++){
    
    int progress = static_cast<float>(i+1)/nEntries*100;
    if((i+1)%interval==0){
      cout<<"Progress: "<< progress+1<<"%\r"<<endl;
      std::cout.flush();
    }

    tree->GetEntry(i);

    eventID=EventID;
    triggerID=TriggerID;
    timecode=TimeCode;

    HitID.clear();
    HitEnergy.clear();
    HitTime.clear();

    for(int i=0;i<25;i++){
      RecHit rechit;
      rechit.Clear();
      rechit.setCrystalID(Hit[i]->CrystalID);

      if(Hit[i]->HighGainPeak>4000){
        double HGA[256],LGA[256],T[256];
        for(int j=0;j<256;j++){
          HGA[j]=Hit[i]->HAmplitude[j];
          LGA[j]=Hit[i]->LAmplitude[j];
          T[j]=j*12.5;
        }

        int MaxID=0;
        //double MaxAmp=0;

        int HGMaxID=0;
        double HGMaxAmp=0;
        
        int LGMaxID=0;
        double LGMaxAmp=0;
       
        for(int j=40;j<80;j++){
          if(HGA[j]>HGMaxAmp){
            HGMaxAmp=HGA[j];
            HGMaxID=j;
          }
        }
        
        for(int j=40;j<80;j++){
          if(LGA[j]>LGMaxAmp){
            LGMaxAmp=LGA[j];
            LGMaxID=j;
          }
        }
        
        if(Hit[i]->HighGainPeak<(Para.HGSatuPoint(i)-400)) MaxID=HGMaxID;
        else MaxID=LGMaxID;

        int cc=40;
        int dd=10;

        VectorXd hgtimingvec(cc);
        VectorXd lgtimingvec(cc);

        for(int j = 0; j < cc; j++){
          hgtimingvec(j)= HGA[MaxID-dd+j];
          lgtimingvec(j)= LGA[MaxID-dd+j];
        }

        double time,amp,pedestal,chi2;

        if(Hit[i]->HighGainPeak<(Para.HGSatuPoint(i)-400)) {
          for(int j = 0; j < 11; j++) f1->SetParameter(j,Para.HGWfPara(i,j));
          //for(int j = 0; j < 11; j++) f1->SetParameter(j,Para.HGWfPara(20,j));
          f1->SetParameter(1,0);
          OnePulseFit(cc,hgtimingvec,&time,&amp,&pedestal,&chi2,f1);
        }
        else {
          for(int j = 0; j < 11; j++) f1->SetParameter(j,Para.LGWfPara(i,j));
          //for(int j = 0; j < 11; j++) f1->SetParameter(j,Para.LGWfPara(20,j));
          f1->SetParameter(1,0);
          OnePulseFit(cc,lgtimingvec,&time,&amp,&pedestal,&chi2,f1);
        }

        rechit.setTime((MaxID)*12.5-dd*12.5+time);
        //cout<<time<<" "<<rechit.Time()<<"  "<<rechit.CrystalID()<<endl;
        HitID.push_back(rechit.CrystalID());
        HitTime.push_back(rechit.Time());
        if((Hit[i]->HighGainPeak-Para.HGPedestal(i))<(Para.HGSatuPoint(i)-400)) rechit.setEnergy(amp*f1->GetMaximum(),Para.LGPedestal(i),true,Para.HGNoise(i),Para.LGNoise(i),Para.Ratio(i),Para.LGMipPeak(i));
        else rechit.setEnergy(amp*f1->GetMaximum(),Para.LGPedestal(i),false,Para.HGNoise(i),Para.LGNoise(i),Para.Ratio(i),Para.LGMipPeak(i));
        HitEnergy.push_back(rechit.Energy());

        //cout<<Hit[i]->HighGainPeak<<"  "<<amp*f1->GetMaximum()<<endl;
        //rchit.setEnergy(Hit[i]->HighGainPeak,Hit[i]->LowGainPeak,Para.HGCali(i),Para.LGCali(i),Para.HGSatuPoint(i),Para.HGPedestal(i),Para.HGNoise(i),Para.LGPedestal(i),Para.LGNoise(i),Para.HGLightYield(i),Para.LGLightYield(i));
      }

      //rechit.setTime();
    }
    rec_data->Fill();
  }

  f->cd();
  rec_data->Write();
  f->Close();
  return 0;

}

bool OnePulseFit(int cc, VectorXd timingvec, double* time, double* amp, double* pedestal, double* ochi2, TF1* f1){

  VectorXd vec_ones(cc);
  MatrixXd mat_noi = MatrixXd::Zero(cc, cc);

  for (int i = 0; i < cc; i++) {
    vec_ones(i) = 1;
    mat_noi(i, i) = 1;
  }

  double t_trg = 0.0;
  //double t_trg = -400;
  VectorXd x(3);
  double fit_a, chi2,pes;
  double deltachi2, tchi2;
  chi2 = 9999;

  for(int l = 0; l < 6; l++){
    VectorXd est_data(cc);
    VectorXd d_est_data(cc);
    for (int i = 0; i < cc; i++) {
      //double i1 = (i * 25 + t_trg);
      double i1 = (i * 12.5 + t_trg);
      est_data(i) = f1->Eval(i1);
      d_est_data(i) = f1->Derivative(i1);
    }
    MatrixXd A(3, 3);
    A(0, 0) = est_data.transpose() * mat_noi * est_data;
    A(0, 1) = est_data.transpose() * mat_noi * d_est_data;
    A(0, 2) = est_data.transpose() * mat_noi * vec_ones;
    A(1, 0) = d_est_data.transpose() * mat_noi * est_data;
    A(1, 1) = d_est_data.transpose() * mat_noi * d_est_data;
    A(1, 2) = d_est_data.transpose() * mat_noi * vec_ones;
    A(2, 0) = vec_ones.transpose() * mat_noi * est_data;
    A(2, 1) = vec_ones.transpose() * mat_noi * d_est_data;
    A(2, 2) = vec_ones.transpose() * mat_noi * vec_ones;

    VectorXd b(3);
    b(0) = est_data.transpose() * mat_noi * timingvec;
    b(1) = d_est_data.transpose() * mat_noi * timingvec;
    b(2) = vec_ones.transpose() * mat_noi * timingvec;
    x = A.colPivHouseholderQr().solve(b);
    double dt;
    dt = x(1) / x(0);
    t_trg = t_trg + dt;
    fit_a = x(0);
    pes = x(2);
    tchi2 = (timingvec - x(0) * est_data - x(1) * d_est_data - x(2) * vec_ones)
      .transpose() *
      mat_noi *
      (timingvec - x(0) * est_data - x(1) * d_est_data - x(2) * vec_ones);
    deltachi2 = chi2 - tchi2;
    chi2 = tchi2; 
  }

  *time=-t_trg;
  *amp=fit_a;
  *pedestal=pes;
  *ochi2=chi2;

  return 1;

}

