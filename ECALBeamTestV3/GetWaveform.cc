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

using namespace std;

bool isTextFile(const string& fileName){
  size_t dotPos = fileName.find_last_of('.');
  if(dotPos == std::string::npos) return false;

  std::string extension = fileName.substr(dotPos + 1);
  if(extension == "txt")
    return true;
  else 
    return false;
}

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
    outputfile = "Waveform.root";
    cout << "Auto save file as Waveform.root..." << endl;
  }

  TFile *f = new TFile(outputfile.Data(),"recreate");

  double T[256],ave[25][256],Ave[25][256];
  TCanvas *c1[25];
  TCanvas *c2[25];
  TGraph *gr1[25];
  TGraph *gr2[25];

  for(int j=0;j<256;j++) T[j]=j*12.5;

  for(int i=0;i<25;i++){
    for(int j=0;j<256;j++){
      ave[i][j]=0; Ave[i][j]=0; 
    }
  }

  const int N=256;

  int CCHg[25]={0};
  int CCLg[25]={0};
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

    for(int k=0;k<25;k++){
  
      if(Hit[k]->LowGainPeak>4000&&Hit[k]->LowGainPeak<16000){
        CCLg[k]++;
        
        double lgpes=0;
        for(int j=0;j<25;j++) lgpes=lgpes+Hit[k]->LAmplitude[j];
        lgpes=lgpes/25;

        for(int j=0;j<256;j++) ave[k][j]=ave[k][j]+(Hit[k]->LAmplitude[j]-lgpes)/(Hit[k]->LowGainPeak-lgpes)*100;
      }
      
      if(Hit[k]->HighGainPeak>4000&&Hit[k]->HighGainPeak<16000){
        CCHg[k]++;
        double hgpes=0;
        for(int j=0;j<25;j++) hgpes=hgpes+Hit[k]->HAmplitude[j];
        hgpes=hgpes/25;

        for(int j=0;j<256;j++) Ave[k][j]=Ave[k][j]+(Hit[k]->HAmplitude[j]-hgpes)/(Hit[k]->HighGainPeak-hgpes)*100;
      }
    }
  }
  
  for(int i=0;i<25;i++){
    for(int j=0;j<256;j++){
      ave[i][j]=ave[i][j]/CCLg[i]; Ave[i][j]=Ave[i][j]/CCHg[i]; 
    }
  }

  double LGAmp[256],HGAmp[256]; 

  for(int k=0;k<25;k++){
    string graphname1="Hit_"+Name[k]+"_LowGainWaveform";
    string graphname2="Hit_"+Name[k]+"_HighGainWaveform";

    for(int j=0;j<256;j++){
      LGAmp[j]=ave[k][j];
      HGAmp[j]=Ave[k][j];
    }
 
    gr1[k] = new TGraph(N,T,LGAmp);
    gr1[k]->SetName(graphname1.data());
    gr1[k]->SetTitle(graphname1.data());
    gr1[k]->GetXaxis()->SetTitle("Time/ns");
    gr1[k]->GetYaxis()->SetTitle("Amp");
    gr1[k]->SetMarkerStyle(20);

    gr2[k] = new TGraph(N,T,HGAmp);
    gr2[k]->SetName(graphname2.data());
    gr2[k]->SetTitle(graphname2.data());
    gr2[k]->GetXaxis()->SetTitle("Time/ns");
    gr2[k]->GetYaxis()->SetTitle("Amp");
    gr2[k]->SetMarkerStyle(20);
      
    string canvasname1="Hit_"+Name[k]+"_LowGainWaveform";
    string canvasname2="Hit_"+Name[k]+"_HighGainWaveform";
    c1[k] = new TCanvas(canvasname1.data(),"",800,600);
    c1[k]->SetGrid();
    c2[k] = new TCanvas(canvasname2.data(),"",800,600);
    c2[k]->SetGrid();

    c1[k]->cd();
    gr1[k]->Draw("AP");
    c2[k]->cd();
    gr2[k]->Draw("AP");

    f->cd();
    gr1[k]->Write();
    gr2[k]->Write();
  }

  f->Close();
  
  return 0;

}
