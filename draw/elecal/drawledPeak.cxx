#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TMath.h>
#include <TString.h>

using namespace std;
using namespace TMath;

void drawledPeak(TString rootname, double cut){


  size_t dospos = rootname.Last('.');
  size_t slashpos = rootname.Last('/');
  TString filename;
  if(dospos != string::npos && slashpos != string::npos){
    filename = rootname(slashpos+1, dospos-slashpos-1);
  }
  else if(dospos != string::npos && slashpos == string::npos){
    filename = rootname(0, dospos);
  }
  else if(dospos == string::npos && slashpos != string::npos){
    filename = rootname(slashpos+1, rootname.Length()-slashpos-1);
  }
  else{
    filename = rootname;
  }
  cout<<"Processing file: "<<rootname<<"("<<filename<<")"<<endl;
  TFile *f1 = new TFile(rootname);
  TTree *t1 = (TTree*)f1->Get("decode_data");
  Int_t nentries = (Int_t)t1->GetEntries();

  vector<vector<Double_t>> vLowGainPedestal;
  vector<vector<Double_t>> vHighGainPedestal;
  vector<vector<Double_t>> vLowGainPeak;
  vector<vector<Double_t>> vHighGainPeak;
  vLowGainPedestal.clear();
  vHighGainPedestal.clear();
  vLowGainPeak.clear();
  vHighGainPeak.clear();

  ifstream infileped("/home/kkbo/beamtest/2025/ECAL/parameter/CaliPara.dat", ios::in);
  ifstream infilemiph("/home/kkbo/beamtest/2025/ECAL/parameter/HGMipPeak.dat", ios::in);
  ifstream infilemipl("/home/kkbo/beamtest/2025/ECAL/parameter/LGMipPeak.dat", ios::in);
  vector<Double_t> vHGratio;
  vector<Double_t> vLGratio;
  vHGratio.clear();
  vLGratio.clear();
  for(Int_t i=0; i<25; i++){
    double val;
    Double_t hped, lped, hgmp, lgmp;
    infileped>>hped>>val>>lped>>val>>val>>val;
    infilemiph>>hgmp;
    infilemipl>>lgmp;  
    vHGratio.push_back((hgmp-hped)/168.0);
    vLGratio.push_back((lgmp-lped)/168.0); 
  }

  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      TTreeReader myReader(t1);
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
          if(k>104 && k<160) continue; //cut the LED injection region
          if(LAmplitude[k] > LPeak) LPeak = LAmplitude[k]; 
        }
        for(size_t k=0; k<HAmplitude.GetSize(); k++){
          if(k>104 && k<160) continue; //cut the LED injection region
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
  
  TH1F *hLowGainMip[5][5];
  TH1F *hHighGainMip[5][5];

  TCanvas *c1 = new TCanvas("c1", "LowGain LED Spectrum", 800, 600);
  c1->Divide(5, 5);
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      c1->cd(21+i-5*j);
      gPad->SetLogy();
      hLowGainMip[i][j] = new TH1F(Form("hLowGainLED_%d_%d", i+1, j+1), Form("LowGain LED Spectrum of Ch%d_%d;LowGain Amplitude [ADC];Entries", i+1, j+1), 100, 0, 600);
      for(size_t k=0; k<vLowGainPedestal[i*5+j].size(); k++){
        Double_t LPed = vLowGainPedestal[i*5+j][k];
        Double_t HPed = vHighGainPedestal[i*5+j][k];
        Double_t LPeak = vLowGainPeak[i*5+j][k];
        Double_t HPeak = vHighGainPeak[i*5+j][k];
        if((LPeak - LPed)/vLGratio[i*5+j] > cut)
          hLowGainMip[i][j]->Fill((LPeak - LPed)/vLGratio[i*5+j]);
      }
      hLowGainMip[i][j]->Draw();
    }
  }
  TCanvas *c2 = new TCanvas("c2", "HighGain MIP Spectrum", 800, 600);
  c2->Divide(5, 5);
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      c2->cd(21+i-5*j);
      gPad->SetLogy();
      hHighGainMip[i][j] = new TH1F(Form("hHighGainLED_%d_%d", i+1, j+1), Form("HighGain LED Spectrum of Ch%d_%d;HighGain Amplitude [ADC];Entries", i+1, j+1), 100, 0, 600);
      for(size_t k=0; k<vHighGainPedestal[i*5+j].size(); k++){
        Double_t LPed = vLowGainPedestal[i*5+j][k];
        Double_t HPed = vHighGainPedestal[i*5+j][k];
        Double_t LPeak = vLowGainPeak[i*5+j][k];
        Double_t HPeak = vHighGainPeak[i*5+j][k];
        if((HPeak - HPed)/vHGratio[i*5+j] > cut)
          hHighGainMip[i][j]->Fill((HPeak - HPed)/vHGratio[i*5+j]);
      }
      hHighGainMip[i][j]->Draw(); 
    }
  }

  TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/LEDPeak_"+filename+".root", "RECREATE");
  fout->cd();
  c1->Write();
  c2->Write();
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      hLowGainMip[i][j]->Write();
      hHighGainMip[i][j]->Write();
    }
  }
  fout->Close();
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv<2)
   {
      cout<<"Usage: ./drawledPeak rootfile cutvalue(default 1000)"<<endl;
      cout<<"  e.g.: ./drawledPeak test.root 1000"<<endl;
      cout<<"  or   : ./drawledPeak (rootlist).txt 1000"<<endl;
      return 1;
   }
   for(Int_t i=0; i<argv; i++){
      cout<<argc[i]<<endl;
   }
    if(string(argc[1]).find(".txt")!=string::npos){
      ifstream infile;
      infile.open(argc[1], ios::in);
      string line;
      while(getline(infile, line)){
        TString rootname=line;
        if(argv==3){
          double cutvalue=stod(argc[2]);
          drawledPeak(rootname, cutvalue);
        }
        else
          drawledPeak(rootname, 1000);
      }
      infile.close();
      return 0;
    }
    else if(string(argc[1]).find(".root")==string::npos){
      cout<<"Please input root file or (rootlist).txt"<<endl;
      return 1;
    }
    TString rootname=argc[1];
    if(argv==3){
      double cutvalue=stod(argc[2]);
      drawledPeak(rootname, cutvalue);
    }
    else
      drawledPeak(rootname, 1000);
    return 0;
}
#endif

