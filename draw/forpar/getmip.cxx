#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TMath.h>
#include <TString.h>

using namespace std;
using namespace TMath;

void getMIP(vector<TString> rootfiles, double cut){

  TChain *chain = new TChain("decode_data");
  for(auto& file : rootfiles){
    chain->Add(file);
    cout<<"Added file: "<<file<<endl;
  }
  int nEntries = chain->GetEntries();
  cout<<"Total entries in chain: "<<nEntries<<endl;

  vector<vector<Double_t>> vLowGainPedestal;
  vector<vector<Double_t>> vHighGainPedestal;
  vector<vector<Double_t>> vLowGainPeak;
  vector<vector<Double_t>> vHighGainPeak;
  vLowGainPedestal.clear();
  vHighGainPedestal.clear();
  vLowGainPeak.clear();
  vHighGainPeak.clear();


  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      cout<<"Processing Channel "<<i+1<<"_"<<j+1<<" ... "<<flush<<endl;
      TTreeReader myReader(chain);
      TTreeReaderValue<Double_t> LowGainPedestal(myReader, Form("Hit_%d_%d.LowGainPedestal", i+1, j+1));
      TTreeReaderValue<Double_t> HighGainPedestal(myReader, Form("Hit_%d_%d.HighGainPedestal", i+1, j+1));
      TTreeReaderArray<Double_t> LAmplitude(myReader, Form("Hit_%d_%d.LAmplitude", i+1, j+1));
      TTreeReaderArray<Double_t> HAmplitude(myReader, Form("Hit_%d_%d.HAmplitude", i+1, j+1));
      vector<Double_t> vLPed;
      vector<Double_t> vHPed;
      vector<Double_t> vLPeak;
      vector<Double_t> vHPeak;   
      vLPed.clear();
      vHPed.clear();
      vLPeak.clear();
      vHPeak.clear();
      while (myReader.Next()) {
        vLPed.push_back(*LowGainPedestal);
        vHPed.push_back(*HighGainPedestal);
        Double_t LPeak = -1;
        Double_t HPeak = -1;
        for(size_t k=0; k<LAmplitude.GetSize(); k++){
          if(LAmplitude[k] > LPeak) LPeak = LAmplitude[k]; 
        }
        for(size_t k=0; k<HAmplitude.GetSize(); k++){
          if(HAmplitude[k] > HPeak) HPeak = HAmplitude[k];  
        }
        vLPeak.push_back(LPeak);
        vHPeak.push_back(HPeak);
      }
      vLowGainPedestal.push_back(vLPed);
      vHighGainPedestal.push_back(vHPed);
      vLowGainPeak.push_back(vLPeak);
      vHighGainPeak.push_back(vHPeak);
    }
  }

  TTree *tree = new TTree("MIP_Spectra", "MIP_Spectra");
  double vLowGainMIP[5][5];
  double vHighGainMIP[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      tree->Branch(Form("LowGainMIP_%d_%d", i+1, j+1), &vLowGainMIP[i][j], Form("LowGainMIP_%d_%d/D", i+1, j+1));
      tree->Branch(Form("HighGainMIP_%d_%d", i+1, j+1), &vHighGainMIP[i][j], Form("HighGainMIP_%d_%d/D", i+1, j+1));
    }
  }
  for(size_t entry=0; entry<nEntries; entry++){
    int bitmask = 0;
    for(Int_t i=0; i<5; i++){
      for(Int_t j=0; j<5; j++){
        Double_t LowGainMIP = vLowGainPeak[5*i+j][entry] - vLowGainPedestal[5*i+j][entry];
        Double_t HighGainMIP = vHighGainPeak[5*i+j][entry] - vHighGainPedestal[5*i+j][entry];
        if(HighGainMIP > cut){
          bitmask += 1;
          vHighGainMIP[i][j] = HighGainMIP;
          vLowGainMIP[i][j] = LowGainMIP;
        }
        else{
          vHighGainMIP[i][j] = -1;
          vLowGainMIP[i][j] = -1;
        }
      }
    }
    if(bitmask!=1) continue;
    tree->Fill();
  } 

  TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/MIP_Spectra.root", "RECREATE");
  tree->Write();
  fout->Close();

}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv<2)
   {
      cout<<"Please input root file or (rootlist).txt or more cutvalue"<<endl;
      return 1;
   }
   for(Int_t i=0; i<argv; i++){
      cout<<argc[i]<<endl;
   }
   vector<TString> rootfiles;
   rootfiles.clear();
   if(string(argc[1]).find(".txt")!=string::npos){
     ifstream infile;
     infile.open(argc[1], ios::in);
     string line;
     while(getline(infile, line)){
       TString rootname=line;
       rootfiles.push_back(rootname);
     }
     infile.close();
   }
   else if(string(argc[1]).find(".root")==string::npos){
     cout<<"Please input root file or (rootlist).txt"<<endl;
     return 1;
   }
    else{
      TString rootname=argc[1];
      rootfiles.push_back(rootname);
    }
   TString rootname=argc[1];
   if(argv==3){
     double cutvalue=stod(argc[2]);
     getMIP(rootfiles, cutvalue);
   }
   else
      getMIP(rootfiles, 1000);
   return 0;
}
#endif

