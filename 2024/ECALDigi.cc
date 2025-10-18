#include <string>
#include <iostream>
#include <fstream>
#include "TString.h"
#include "data_model.hh"
#include <TFile.h>
#include <TProfile.h>
#include <TTree.h>
#include <TH1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TF1.h>
#include "TChain.h"

#include "Parameter.hh"
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
bool PipelineFit(int cc, VectorXd timingvec, vector<double>& Vectime , vector<double>& Vecamp, vector<double>& Vecchi2, TF1* f1, double noise, double ADCE_ratio);
double GetChi2(Eigen::VectorXd timingvec, Eigen::MatrixXd mat_noi, double time, double amp, int m_npoint, int m_begpoint, TF1* f1);
bool FastPulseFit(VectorXd timingvec, double* otime, double* oamp, double* ochi2, MatrixXd mat_noi, MatrixXd M, VectorXd fvec, VectorXd dfvec, int m_npoint, int m_begpoint, TF1* f1);
Eigen::VectorXd MoveTemplate(Eigen::VectorXd ADCvec, double time, double amp, double sampletime, TF1* f1);

int main(int argc, char const *argv[]){
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
  int TriggerID;
  tree->SetBranchAddress("TriggerID",&TriggerID);
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
    outputfile = "ECALDigi.root";
    cout << "Auto save file as ECALDigi.root..." << endl;
  }

  TFile *f = new TFile(outputfile.Data(),"recreate");

  int triggerID;
  vector<int> CrystalID;
  vector<double> TimeHG;
  vector<double> TimeLG;
  vector<double> AmpHG_Peak; //ADC for High Gain from peak finding
  vector<double> AmpLG_Peak; //ADC for Low Gain from peak finding
  vector<double> AmpHG_Fit; //ADC for High Gain from fitting
  vector<double> AmpLG_Fit; //ADC for Low Gain from fitting
  vector<int> Diginumber;
  vector<vector<double>> AmpHG_FitVec;
  vector<vector<double>> AmpLG_FitVec;
  vector<vector<double>> TimeHGVec;
  vector<vector<double>> TimeLGVec;
  for(int i=0;i<25;i++){
    vector<double> temp;
    AmpHG_FitVec.push_back(temp);
    AmpLG_FitVec.push_back(temp);
    TimeHGVec.push_back(temp);
    TimeLGVec.push_back(temp);
  }


  TTree *ECALdigi_data = new TTree("ECALdigi_data","ECALdigi_data");
  ECALdigi_data->Branch("EventID",&triggerID,"triggerID/I");
  ECALdigi_data->Branch("CrystalID",&CrystalID);
  ECALdigi_data->Branch("TimeLG",&TimeLG);
  ECALdigi_data->Branch("TimeHG",&TimeHG);
  ECALdigi_data->Branch("AmpHG_Peak",&AmpHG_Peak);
  ECALdigi_data->Branch("AmpLG_Peak",&AmpLG_Peak);
  ECALdigi_data->Branch("AmpHG_Fit",&AmpHG_Fit);
  ECALdigi_data->Branch("AmpLG_Fit",&AmpLG_Fit);
  ECALdigi_data->Branch("Diginumber",&Diginumber);
  for(int i=0;i<25;i++){
    string name="AmpHG_Fit_"+Name[i];
    ECALdigi_data->Branch(name.data(),&AmpHG_FitVec[i]);
    name="AmpLG_Fit_"+Name[i];
    ECALdigi_data->Branch(name.data(),&AmpLG_FitVec[i]);
    name="TimeHG_"+Name[i];
    ECALdigi_data->Branch(name.data(),&TimeHGVec[i]);
    name="TimeLG_"+Name[i];
    ECALdigi_data->Branch(name.data(),&TimeLGVec[i]);
  }

  Parameter& Para=Parameter::GetInstance();

  TF1* f1 = new TF1("f",ff,0,2000,11);

  int nEntries=tree->GetEntries();
  int interval = nEntries/20;

  //for(int i=0;i<nEntries;i++){
    for(int i=0;i<10;i++){
    int progress = static_cast<float>(i+1)/nEntries*100;
    if((i+1)%interval==0){
      cout<<"Progress: "<< progress+1<<"%\r"<<endl;
      std::cout.flush();
    }

    tree->GetEntry(i);

    triggerID=TriggerID;

    CrystalID.clear();
    TimeHG.clear();
    TimeLG.clear();
    AmpHG_Peak.clear();
    AmpLG_Peak.clear();
    AmpHG_Fit.clear();
    AmpLG_Fit.clear();
    Diginumber.clear();

    for(int k=0;k<25;k++){
      CrystalID.push_back(Hit[k]->CrystalID);
      //AmpHG_Peak.push_back(Hit[k]->HighGainPeak);
      //AmpLG_Peak.push_back(Hit[k]->LowGainPeak);

      AmpHG_FitVec[k].clear();
      AmpLG_FitVec[k].clear();
      TimeHGVec[k].clear();
      TimeLGVec[k].clear();
      
      //Fit
      double HGA[256],LGA[256],T[256];
      for(int j=0;j<256;j++){
        HGA[j]=Hit[k]->HAmplitude[j];
        LGA[j]=Hit[k]->LAmplitude[j];
        T[j]=j*12.5;
      }

      int HGMaxID=0;
      double HGMaxAmp=0;

      int LGMaxID=0;
      double LGMaxAmp=0;

      for(int j=40;j<240;j++){
        if(HGA[j]>=HGMaxAmp){
          HGMaxAmp=HGA[j];
          HGMaxID=j;
        }
      }

      for(int j=40;j<240;j++){
        if(LGA[j]>=LGMaxAmp){
          LGMaxAmp=LGA[j];
          LGMaxID=j;
        }
      }

      AmpHG_Peak.push_back(HGMaxAmp);
      AmpLG_Peak.push_back(LGMaxAmp);

      //cout<<i<<"  "<<HGMaxID<<endl;

      bool IsOsc=false;
      //if((HGMaxID<56||HGMaxID>60)&&HGMaxAmp<16000) IsOsc=true;

      int cc=40;
      int dd=10;
      int nfitpoint=18;

      VectorXd hgtimingvec(cc);
      VectorXd lgtimingvec(cc);

      VectorXd hpipelinevec(256);
      VectorXd lpipelinevec(256);

      for(int j = 0; j < cc; j++){
        hgtimingvec(j)= HGA[HGMaxID-dd+j];
        lgtimingvec(j)= LGA[LGMaxID-dd+j];
      }

      double time,amp,pedestal,chi2;
      bool ifpipeline = true;

      if(ifpipeline){
        if(HGMaxAmp<16000){
          for(int j = 0; j < 11; j++) f1->SetParameter(j,Para.HGWfPara(k,j));
          f1->SetParameter(1,0);
          for(int j = 0; j < 256; j++) hpipelinevec(j)=HGA[j]-Para.HGPedestal(k);
          vector<double> Vectime;
          vector<double> Vecamp;
          vector<double> Vecchi2;
          Vectime.clear();
          Vecamp.clear();
          Vecchi2.clear();
          double ADCE_ratio=Para.Ratio(k)*(Para.LGMipPeak(k)-Para.LGPedestal(k))/168.0;
          PipelineFit(nfitpoint,hpipelinevec,Vectime,Vecamp,Vecchi2,f1,Para.HGNoise(k),ADCE_ratio);
          double maxamp=0;
          double maxindex=0;
          for(int j = 0; j < Vecamp.size(); j++){
            if(Vecamp.at(j)>maxamp){
              maxamp=Vecamp.at(j);
              maxindex=j;
            }
          }
          if(Vecamp.size()!=0){
            TimeHG.push_back(Vectime.at(maxindex));
            AmpHG_Fit.push_back(Vecamp.at(maxindex));
            Diginumber.push_back(Vecamp.size());
            AmpHG_FitVec[k]=Vecamp;
            TimeHGVec[k]=Vectime;
          }
          else{
            TimeHG.push_back(888888);
            AmpHG_Fit.push_back(0);
            Diginumber.push_back(0);
          }
        }
        else{
          TimeHG.push_back(888888);
          AmpHG_Fit.push_back(HGMaxAmp-Para.HGPedestal(k));
          Diginumber.push_back(1);
          AmpHG_FitVec[k].push_back(HGMaxAmp-Para.HGPedestal(k));
          TimeHGVec[k].push_back(888888);
        }
        if(LGMaxAmp<16000){
          for(int j = 0; j < 11; j++) f1->SetParameter(j,Para.LGWfPara(k,j));
          f1->SetParameter(1,0);
          for(int j = 0; j < 256; j++) lpipelinevec(j)=LGA[j]-Para.LGPedestal(k);
          vector<double> Vectime;
          vector<double> Vecamp;
          vector<double> Vecchi2;
          Vectime.clear();
          Vecamp.clear();  
          Vecchi2.clear();
          double ADCE_ratio=(Para.LGMipPeak(k)-Para.LGPedestal(k))/168.0;
          PipelineFit(nfitpoint,lpipelinevec,Vectime,Vecamp,Vecchi2,f1,Para.LGNoise(k),ADCE_ratio);
          double maxamp=0;
          double maxindex=0;
          for(int j = 0; j < Vecamp.size(); j++){
            if(Vecamp.at(j)>maxamp){
              maxamp=Vecamp.at(j);
              maxindex=j;
            }
          }
          if(Vecamp.size()!=0){
            TimeLG.push_back(Vectime.at(maxindex));
            AmpLG_Fit.push_back(Vecamp.at(maxindex));
            Diginumber.push_back(Vecamp.size());
            AmpLG_FitVec[k]=Vecamp;
            TimeLGVec[k]=Vectime;
          }
          else{
            TimeLG.push_back(888888);
            AmpLG_Fit.push_back(0);
            Diginumber.push_back(0);
          }
        }
        else{
          TimeLG.push_back(888888);
          AmpLG_Fit.push_back(LGMaxAmp-Para.LGPedestal(k));
          Diginumber.push_back(1);
          AmpLG_FitVec[k].push_back(LGMaxAmp-Para.LGPedestal(k));
          TimeLGVec[k].push_back(888888);
        }
      }
      else{
        if(Hit[k]->HighGainPeak>(Para.HGPedestal(k)+6*Para.HGNoise(k))&&Hit[k]->HighGainPeak<16000&&IsOsc==false) {
          for(int j = 0; j < 11; j++) f1->SetParameter(j,Para.HGWfPara(k,j));
          f1->SetParameter(1,0);
          OnePulseFit(cc,hgtimingvec,&time,&amp,&pedestal,&chi2,f1);
          TimeHG.push_back((HGMaxID)*12.5-dd*12.5+time);
          AmpHG_Fit.push_back(amp*f1->GetMaximum());
        }
        else{
          if(IsOsc==false)
          TimeHG.push_back(999999);
          else 
          TimeHG.push_back(888888);
          AmpHG_Fit.push_back(Hit[k]->HighGainPeak-Para.HGPedestal(k));
          //AmpHG_Fit.push_back(0);
        }

        if(Hit[k]->LowGainPeak>(Para.LGPedestal(k)+6*Para.LGNoise(k))&&Hit[k]->LowGainPeak<16000&&IsOsc==false){
          for(int j = 0; j < 11; j++) f1->SetParameter(j,Para.LGWfPara(k,j));
          f1->SetParameter(1,0);
          OnePulseFit(cc,lgtimingvec,&time,&amp,&pedestal,&chi2,f1);
          TimeLG.push_back((LGMaxID)*12.5-dd*12.5+time);
          AmpLG_Fit.push_back(amp*f1->GetMaximum());
        }
        else{
          if(IsOsc==false)
          TimeLG.push_back(999999);
          else
          TimeLG.push_back(888888);
          AmpLG_Fit.push_back(Hit[k]->LowGainPeak-Para.LGPedestal(k));
          //AmpLG_Fit.push_back(0);
        }
      }

      cout<<i<<" "<<k<<" "<<HGMaxAmp-Para.HGPedestal(k)<<" "<<5*Para.HGNoise(k)<<"  "<<AmpHG_Fit.at(k)<<"  "<<TimeHG.at(k)<<endl;
      cout<<i<<" "<<k<<" "<<LGMaxAmp-Para.LGPedestal(k)<<" "<<5*Para.LGNoise(k)<<"  "<<AmpLG_Fit.at(k)<<"  "<<TimeLG.at(k)<<endl;
      //cout<<i<<" "<<k<<" "<<Hit[k]->HighGainPeak<<"  "<<Hit[k]->HighGainPeak-Para.HGPedestal(k)<<"  "<<AmpHG_Fit.at(k)<<endl;
      //cout<<i<<" "<<k<<" "<<Hit[k]->LowGainPeak<<"  "<<Hit[k]->LowGainPeak-Para.LGPedestal(k)<<"  "<<AmpLG_Fit.at(k)<<endl;

    }

    ECALdigi_data->Fill();
  }

  f->cd();
  ECALdigi_data->Write();
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

bool PipelineFit(int cc, VectorXd timingvec, vector<double>& Vectime , vector<double>& Vecamp, vector<double>& Vecchi2, TF1* f1, double noise, double ADCE_ratio){

  MatrixXd mat_noi = MatrixXd::Zero(cc, cc);
  for (int i = 0; i < cc; i++) {
    mat_noi(i, i) = 1;
  }
  
  VectorXd f1vec(cc);
  VectorXd df1vec(cc);
  VectorXd f2vec(cc);
  VectorXd df2vec(cc);

  int m_begpoint = 4;
  double sampletime = 12.5;
  int m_npoint = cc;

  for (int i = 0; i < m_npoint; i++) {
    double i1 = ((i+m_begpoint) * sampletime);
    f1vec(i) = f1->Eval(i1);
    df1vec(i) = f1->Derivative(i1);
    double i2 = ((i+m_begpoint) * sampletime-sampletime/2);
    f2vec(i) = f1->Eval(i2);
    df2vec(i) = f1->Derivative(i2);
  }

  MatrixXd M0(2,2);
  M0(0,0) = f1vec.transpose() * mat_noi * f1vec;
  M0(0,1) = f1vec.transpose() * mat_noi * df1vec;
  M0(1,0) = df1vec.transpose() * mat_noi * f1vec;
  M0(1,1) = df1vec.transpose() * mat_noi * df1vec;
  MatrixXd M1 = M0.inverse();

  M0(0,0) = f2vec.transpose() * mat_noi * f2vec;
  M0(0,1) = f2vec.transpose() * mat_noi * df2vec;
  M0(1,0) = df2vec.transpose() * mat_noi * f2vec;
  M0(1,1) = df2vec.transpose() * mat_noi * df2vec;
  MatrixXd M2 = M0.inverse();

  int nSamplePoints = timingvec.size() - m_npoint + 1;
  Eigen::VectorXd ADCvec = timingvec;
  vector<double> parvec;
  parvec.clear();
  int nsubfit = 2;
  int index = 0;
  for (int i = 0; i < nSamplePoints; i++) {
    double time, amp, chi2;
    Eigen::VectorXd timingvec_sub(m_npoint);
    for (int k = 0; k < nsubfit; k++) {
      for (int j = 0; j < m_npoint; j++) {
        timingvec_sub(j) = ADCvec(i + j);
      }
      bool ifFit = true;
      if(k==0) FastPulseFit(timingvec_sub, &time, &amp, &chi2, mat_noi, M1, f1vec, df1vec, m_npoint, m_begpoint, f1);
      else FastPulseFit(timingvec_sub, &time, &amp, &chi2, mat_noi, M2, f2vec, df2vec, m_npoint, m_begpoint, f1);
      if(time<-sampletime/2) ifFit = false;
      if(time>sampletime/2) ifFit = false;
      time = time + (i-m_begpoint)*sampletime+k*sampletime/2;
      double amp_energy;
      double chindf;
      chi2 = chi2/pow(ADCE_ratio,2);
      amp_energy = amp*f1->GetMaximum()/ADCE_ratio;
      chindf = chi2/((m_npoint-2)*(pow(noise/ADCE_ratio,2)+pow(amp_energy*0.01,2)));
      //cout<<"chi2: "<<chi2<<"  chindf: "<<chindf<<"  amp: "<<amp*f1->GetMaximum()<<"  time: "<<time<<"  detaltime:  "<<time-(i-m_begpoint)*sampletime-k*sampletime/2<<"  amp_energy: "<<amp_energy<<"  noise: "<<noise<<"  ADCE_ratio: "<<ADCE_ratio<<endl;
      if(chindf>3) ifFit = false;
      double Ethr = noise*3.0;
      if(amp*f1->GetMaximum()<Ethr) ifFit = false;
      if(index+1==2*i+k&&parvec.size()!=0){
        if(ifFit&&chindf<parvec[2]){
          Vectime.push_back(time);
          Vecamp.push_back(amp*f1->GetMaximum());
          Vecchi2.push_back(chi2);
          ADCvec = MoveTemplate(ADCvec, time, amp, sampletime, f1);
          parvec.clear();
        }
        else{
          Vectime.push_back(parvec[0]);
          Vecamp.push_back(parvec[1]*f1->GetMaximum());
          Vecchi2.push_back(parvec[2]);
          ADCvec = MoveTemplate(ADCvec, parvec[0], parvec[1], sampletime, f1);
          parvec.clear();
        }
      }
      else if(ifFit){
        parvec.clear();
        parvec.push_back(time);
        parvec.push_back(amp);
        parvec.push_back(chindf);
        parvec.push_back(amp_energy/1000.0);
        index = 2*i+k;
      }
    } 
  }

  return 1;
}

bool FastPulseFit(VectorXd timingvec, double* otime, double* oamp, double* ochi2, MatrixXd mat_noi, MatrixXd M, VectorXd fvec, VectorXd dfvec, int m_npoint, int m_begpoint, TF1* f1){
  double t_trg = 0.0;
  VectorXd x;
  double fit_a, chi2;
  VectorXd b(2);
  b(0) = fvec.transpose() * mat_noi * timingvec;
  b(1) = dfvec.transpose() * mat_noi * timingvec;
  x = M * b;
  t_trg = x(1) / x(0);
  fit_a = x(0);
  chi2 = GetChi2(timingvec, mat_noi, t_trg, fit_a, m_npoint, m_begpoint, f1);
  *otime = -t_trg;
  *oamp = fit_a;
  *ochi2 = chi2;
  return 1;
}

Eigen::VectorXd MoveTemplate(Eigen::VectorXd ADCvec, double time, double amp, double sampletime, TF1* f1){
  Eigen::VectorXd ADCvec_new(ADCvec.size());
  for (int i = 0; i < ADCvec.size(); i++) {
    if(i*sampletime-time<-100||i*sampletime-time>1000) ADCvec_new(i) = ADCvec(i);
    else ADCvec_new(i) = ADCvec(i)-amp*f1->Eval(i*sampletime-time);
  }
  return ADCvec_new;
}

double GetChi2(Eigen::VectorXd timingvec, Eigen::MatrixXd mat_noi, double time, double amp, int m_npoint, int m_begpoint, TF1* f1){
    VectorXd fitvec(m_npoint);
    for (int i = 0; i < m_npoint; i++) {
      double i1 = ((i+m_begpoint) * 12.5 + time);
      fitvec(i) = f1->Eval(i1);
    }
    double chi2 = (timingvec - amp * fitvec)
      .transpose() *
      mat_noi *
      (timingvec - amp * fitvec);
    return chi2;
}

