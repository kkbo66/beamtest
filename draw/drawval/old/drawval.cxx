#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLine.h>
#include <TF1.h>
#include <TMath.h>
#include <TString.h>
#include <TAxis.h>

using namespace std;
using namespace TMath;

void drawval(TString rootname){

  //gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetLegendFont(42);
  gStyle->SetTextFont(42);
  gStyle->SetTitleSize(0.05,"xyz");
  gStyle->SetLabelSize(0.04,"xyz");
  gStyle->SetLegendTextSize(0.04);
  gStyle->SetTitleOffset(1.2,"y");
  gStyle->SetTitleOffset(1.0,"x");

  TFile *f1 = new TFile("/home/kkbo/beamtest/2025/ECAL/build/"+rootname);
  TTree *t1 = (TTree*)f1->Get("decode_data");
  Int_t nentries = (Int_t)t1->GetEntries();

  //const Int_t nPoints = 256;
  TH1F *hTriggerID;
  TH1F *hTime;
  TH1F *hEventID;
  TH1F *hTemp1[5][5];
  TH1F *hTemp2[5][5];
  TH1F *hLAmpl[5][5];
  TH1F *hHAmpl[5][5];
  TH1F *hLNoise[5][5];
  TH1F *hHNoise[5][5];
  TH1F *hLPed[5][5];
  TH1F *hHPed[5][5];
  TH1F *hLPeak[5][5];
  TH1F *hHPeak[5][5];
  TGraph *gLAmplTime[5][5];
  TGraph *gHAmplTime[5][5];
  TGraph *gLNoiseTime[5][5];
  TGraph *gHNoiseTime[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      gLAmplTime[i][j] = new TGraph();
      gHAmplTime[i][j] = new TGraph();
      gLNoiseTime[i][j] = new TGraph();
      gHNoiseTime[i][j] = new TGraph();
    }
  } 
  
  TTreeReader myReader0(t1);
  TTreeReaderValue<Long64_t> EventID(myReader0, "EventID");
  TTreeReaderValue<Int_t> TriggerID0(myReader0, "TriggerID");
  TTreeReaderArray<Float_t> Time(myReader0, "Time");
  vector<Long64_t> vEventID;
  vector<Int_t> vTriggerID;
  vector<Float_t> vTime;
  vEventID.clear();
  vTriggerID.clear();
  vTime.clear();
  while (myReader0.Next()) {
    for(Int_t i=0; i<5; i++){
      for(Int_t j=0; j<5; j++){
        vEventID.push_back(*EventID);
        vTriggerID.push_back(*TriggerID0);
        for(Int_t k=0; k<Time.GetSize(); k++){
          vTime.push_back(Time[k]);
        }
      }
    }
  }
  
  double minTime = 9999;
  double maxTime = -9999;
  for(Int_t i=0; i<vTime.size(); i++){
    if(vTime[i]<minTime) minTime = vTime[i];
    if(vTime[i]>maxTime) maxTime = vTime[i];
  }
  double minEventID = 9999;
  double maxEventID = -9999;
  for(Int_t i=0; i<vEventID.size(); i++){
    if(vEventID[i]<minEventID) minEventID = vEventID[i];
    if(vEventID[i]>maxEventID) maxEventID = vEventID[i];
  }
  double minTriggerID = 9999;
  double maxTriggerID = -9999;
  for(Int_t i=0; i<vTriggerID.size(); i++){
    if(vTriggerID[i]<minTriggerID) minTriggerID = vTriggerID[i];
    if(vTriggerID[i]>maxTriggerID) maxTriggerID = vTriggerID[i];
  } 
  hEventID = new TH1F("hEventID", "EventID;EventID;Counts", (Int_t)(maxEventID-minEventID+1), minEventID-0.5, maxEventID+0.5);
  hTriggerID = new TH1F("hTriggerID", "TriggerID;TriggerID;Counts", (Int_t)(maxTriggerID-minTriggerID+1), minTriggerID-0.5, maxTriggerID+0.5);
  hTime = new TH1F("hTime", "Time;Time(ns);Counts", 100, minTime, maxTime);

  for(Int_t i=0; i<vTime.size(); i++){
    hTime->Fill(vTime[i]);
  }
  for(Int_t i=0; i<vEventID.size(); i++){
    hEventID->Fill(vEventID[i]);
  }
  for(Int_t i=0; i<vTriggerID.size(); i++){
    hTriggerID->Fill(vTriggerID[i]);
  }

  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      TTreeReader myReader(t1);
      TTreeReaderValue<Long64_t> CrystalID(myReader, Form("Hit_%d_%d.CrystalID", i+1, j+1));
      TTreeReaderValue<Double_t> Temperature1(myReader, Form("Hit_%d_%d.Temperature1", i+1, j+1));
      TTreeReaderValue<Double_t> Temperature2(myReader, Form("Hit_%d_%d.Temperature2", i+1, j+1));
      TTreeReaderValue<Double_t> LowGainPedestal(myReader, Form("Hit_%d_%d.LowGainPedestal", i+1, j+1));
      TTreeReaderValue<Double_t> HighGainPedestal(myReader, Form("Hit_%d_%d.HighGainPedestal", i+1, j+1));
      TTreeReaderValue<Double_t> LowGainPeak(myReader, Form("Hit_%d_%d.LowGainPeak", i+1, j+1));
      TTreeReaderValue<Double_t> HighGainPeak(myReader, Form("Hit_%d_%d.HighGainPeak", i+1, j+1));
      TTreeReaderArray<Double_t> LAmplitude(myReader, Form("Hit_%d_%d.LAmplitude", i+1, j+1));
      TTreeReaderArray<Double_t> HAmplitude(myReader, Form("Hit_%d_%d.HAmplitude", i+1, j+1));
      TTreeReaderArray<Double_t> LNoise(myReader, Form("Hit_%d_%d.LNoise", i+1, j+1));
      TTreeReaderArray<Double_t> HNoise(myReader, Form("Hit_%d_%d.HNoise", i+1, j+1));
      vector<Int_t> vTriggerID;
      vector<Long64_t> vCrystalID;
      vector<Double_t> vTemp1;
      vector<Double_t> vTemp2;
      vector<Double_t> vLAmpl;
      vector<Double_t> vHAmpl;
      vector<Double_t> vLNoise;
      vector<Double_t> vHNoise;
      vector<Double_t> vLPed;
      vector<Double_t> vHPed;
      vector<Double_t> vLPeak;
      vector<Double_t> vHPeak;   
      vTriggerID.clear();   
      vCrystalID.clear();
      vTemp1.clear();
      vTemp2.clear();
      vLAmpl.clear();
      vHAmpl.clear();
      vLNoise.clear();
      vHNoise.clear();
      vLPed.clear();
      vHPed.clear();
      vLPeak.clear();
      vHPeak.clear();
      while (myReader.Next()) {
        vCrystalID.push_back(*CrystalID);
        vTemp1.push_back(*Temperature1);
        vTemp2.push_back(*Temperature2);
        vLPed.push_back(*LowGainPedestal);
        vHPed.push_back(*HighGainPedestal);
        vLPeak.push_back(*LowGainPeak);
        vHPeak.push_back(*HighGainPeak);
        for(Int_t k=0; k<LAmplitude.GetSize(); k++){
          vLAmpl.push_back(LAmplitude[k]);
          vHAmpl.push_back(HAmplitude[k]);
          gLAmplTime[i][j]->SetPoint(gLAmplTime[i][j]->GetN(), k*12.5, LAmplitude[k]);
          gHAmplTime[i][j]->SetPoint(gHAmplTime[i][j]->GetN(), k*12.5, HAmplitude[k]);
        }
        for(Int_t k=0; k<LNoise.GetSize(); k++){
          vLNoise.push_back(LNoise[k]);
          vHNoise.push_back(HNoise[k]);
          gLNoiseTime[i][j]->SetPoint(gLNoiseTime[i][j]->GetN(), k*12.5, LNoise[k]);
          gHNoiseTime[i][j]->SetPoint(gHNoiseTime[i][j]->GetN(), k*12.5, HNoise[k]);
        }
      }
      double minTemp1 = 9999;
      double maxTemp1 = -9999;
      for(Int_t k=0; k<vTemp1.size(); k++){
        if(vTemp1[k]<minTemp1) minTemp1 = vTemp1[k];
        if(vTemp1[k]>maxTemp1) maxTemp1 = vTemp1[k];
      }
      double minTemp2 = 9999;
      double maxTemp2 = -9999;
      for(Int_t k=0; k<vTemp2.size(); k++){
        if(vTemp2[k]<minTemp2) minTemp2 = vTemp2[k];
        if(vTemp2[k]>maxTemp2) maxTemp2 = vTemp2[k];
      }
      double minLPed = 9999;
      double maxLPed = -9999;
      for(Int_t k=0; k<vLPed.size(); k++){
        if(vLPed[k]<minLPed) minLPed = vLPed[k];
        if(vLPed[k]>maxLPed) maxLPed = vLPed[k];
      }
      double minHPed = 9999;
      double maxHPed = -9999;
      for(Int_t k=0; k<vHPed.size(); k++){
        if(vHPed[k]<minHPed) minHPed = vHPed[k];
        if(vHPed[k]>maxHPed) maxHPed = vHPed[k];
      }
      double minLPeak = 9999;
      double maxLPeak = -9999;
      for(Int_t k=0; k<vLPeak.size(); k++){
        if(vLPeak[k]<minLPeak) minLPeak = vLPeak[k];
        if(vLPeak[k]>maxLPeak) maxLPeak = vLPeak[k];
      }
      double minHPeak = 9999;
      double maxHPeak = -9999;
      for(Int_t k=0; k<vHPeak.size(); k++){
        if(vHPeak[k]<minHPeak) minHPeak = vHPeak[k];
        if(vHPeak[k]>maxHPeak) maxHPeak = vHPeak[k];
      }
      double minLAmpl = 9999;
      double maxLAmpl = -9999;
      for(Int_t k=0; k<vLAmpl.size(); k++){
        if(vLAmpl[k]<minLAmpl) minLAmpl = vLAmpl[k];
        if(vLAmpl[k]>maxLAmpl) maxLAmpl = vLAmpl[k];
      }
      double minHAmpl = 9999;
      double maxHAmpl = -9999;
      for(Int_t k=0; k<vHAmpl.size(); k++){
        if(vHAmpl[k]<minHAmpl) minHAmpl = vHAmpl[k];
        if(vHAmpl[k]>maxHAmpl) maxHAmpl = vHAmpl[k];
      }
      double minLNoise = 9999;
      double maxLNoise = -9999;
      for(Int_t k=0; k<vLNoise.size(); k++){
        if(vLNoise[k]<minLNoise) minLNoise = vLNoise[k];
        if(vLNoise[k]>maxLNoise) maxLNoise = vLNoise[k];
      }
      double minHNoise = 9999;
      double maxHNoise = -9999;
      for(Int_t k=0; k<vHNoise.size(); k++){
        if(vHNoise[k]<minHNoise) minHNoise = vHNoise[k];
        if(vHNoise[k]>maxHNoise) maxHNoise = vHNoise[k];
      }
      hTemp1[i][j] = new TH1F(Form("hTemp1_%d_%d", i+1, j+1), Form("Temperature1 Crystal (%d,%d);Temperature1(^{o}C);Counts", i+1, j+1), (Int_t)(maxTemp1-minTemp1+1), minTemp1-0.5, maxTemp1+0.5);
      hTemp2[i][j] = new TH1F(Form("hTemp2_%d_%d", i+1, j+1), Form("Temperature2 Crystal (%d,%d);Temperature2(^{o}C);Counts", i+1, j+1), (Int_t)(maxTemp2-minTemp2+1), minTemp2-0.5, maxTemp2+0.5);
      hLPed[i][j] = new TH1F(Form("hLPed_%d_%d", i+1, j+1), Form("Low Gain Pedestal Crystal (%d,%d);Low Gain Pedestal;Counts", i+1, j+1), (Int_t)(maxLPed-minLPed+1), minLPed-0.5, maxLPed+0.5);
      hHPed[i][j] = new TH1F(Form("hHPed_%d_%d", i+1, j+1), Form("High Gain Pedestal Crystal (%d,%d);High Gain Pedestal;Counts", i+1, j+1), (Int_t)(maxHPed-minHPed+1), minHPed-0.5, maxHPed+0.5);
      hLPeak[i][j] = new TH1F(Form("hLPeak_%d_%d", i+1, j+1), Form("Low Gain Peak Crystal (%d,%d);Low Gain Peak;Counts", i+1, j+1), (Int_t)(maxLPeak-minLPeak+1), minLPeak-0.5, maxLPeak+0.5);
      hHPeak[i][j] = new TH1F(Form("hHPeak_%d_%d", i+1, j+1), Form("High Gain Peak Crystal (%d,%d);High Gain Peak;Counts", i+1, j+1), (Int_t)(maxHPeak-minHPeak+1), minHPeak-0.5, maxHPeak+0.5);
      hLAmpl[i][j] = new TH1F(Form("hLAmpl_%d_%d", i+1, j+1), Form("Low Gain Amplitude Crystal (%d,%d);Low Gain Amplitude;Counts", i+1, j+1), (Int_t)(maxLAmpl-minLAmpl+1), minLAmpl-0.5, maxLAmpl+0.5);
      hHAmpl[i][j] = new TH1F(Form("hHAmpl_%d_%d", i+1, j+1), Form("High Gain Amplitude Crystal (%d,%d);High Gain Amplitude;Counts", i+1, j+1), (Int_t)(maxHAmpl-minHAmpl+1), minHAmpl-0.5, maxHAmpl+0.5);
      hLNoise[i][j] = new TH1F(Form("hLNoise_%d_%d", i+1, j+1), Form("Low Gain Noise Crystal (%d,%d);Low Gain Noise;Counts", i+1, j+1), (Int_t)(maxLNoise-minLNoise+1), minLNoise-0.5, maxLNoise+0.5);
      hHNoise[i][j] = new TH1F(Form("hHNoise_%d_%d", i+1, j+1), Form("High Gain Noise Crystal (%d,%d);High Gain Noise;Counts", i+1, j+1), (Int_t)(maxHNoise-minHNoise+1), minHNoise-0.5, maxHNoise+0.5);
      for(Int_t k=0; k<vTemp1.size(); k++){
        hTemp1[i][j]->Fill(vTemp1[k]);
      }
      for(Int_t k=0; k<vTemp2.size(); k++){
        hTemp2[i][j]->Fill(vTemp2[k]);
      } 
      for(Int_t k=0; k<vLPed.size(); k++){
        hLPed[i][j]->Fill(vLPed[k]);
      }
      for(Int_t k=0; k<vHPed.size(); k++){
        hHPed[i][j]->Fill(vHPed[k]);
      }
      for(Int_t k=0; k<vLPeak.size(); k++){
        hLPeak[i][j]->Fill(vLPeak[k]);
      }
      for(Int_t k=0; k<vHPeak.size(); k++){
        hHPeak[i][j]->Fill(vHPeak[k]);
      }
      for(Int_t k=0; k<vLAmpl.size(); k++){
        hLAmpl[i][j]->Fill(vLAmpl[k]);
      }
      for(Int_t k=0; k<vHAmpl.size(); k++){
        hHAmpl[i][j]->Fill(vHAmpl[k]);
      }
      for(Int_t k=0; k<vLNoise.size(); k++){
        hLNoise[i][j]->Fill(vLNoise[k]);
      }
      for(Int_t k=0; k<vHNoise.size(); k++){
        hHNoise[i][j]->Fill(vHNoise[k]);
      }
    }
  }

  TCanvas *cTime = new TCanvas("cTime", "Time", 800, 600);
  hTime->Draw();
  TCanvas *cEventID = new TCanvas("cEventID", "EventID", 800, 600);
  hEventID->Draw();
  TCanvas *cTriggerID = new TCanvas("cTriggerID", "TriggerID", 800, 600);
  hTriggerID->Draw();
  TCanvas *cTemp1 = new TCanvas("cTemp1", "Temperature1", 800, 600);
  cTemp1->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cTemp1->cd(i*5+j+1);
      hTemp1[i][j]->Draw();
    }
  }
  TCanvas *cTemp2 = new TCanvas("cTemp2", "Temperature2", 800, 600);
  cTemp2->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cTemp2->cd(i*5+j+1);
      hTemp2[i][j]->Draw();
    }
  }
  TCanvas *cLPed = new TCanvas("cLPed", "Low Gain Pedestal", 800, 600);
  cLPed->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cLPed->cd(i*5+j+1);
      hLPed[i][j]->Draw();
    }
  }
  TCanvas *cHPed = new TCanvas("cHPed", "High Gain Pedestal", 800, 600);
  cHPed->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cHPed->cd(i*5+j+1);
      hHPed[i][j]->Draw();
    }
  }
  TCanvas *cLPeak = new TCanvas("cLPeak", "Low Gain Peak", 800, 600);
  cLPeak->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cLPeak->cd(i*5+j+1);
      hLPeak[i][j]->Draw();
    }
  }
  TCanvas *cHPeak = new TCanvas("cHPeak", "High Gain Peak", 800, 600);
  cHPeak->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cHPeak->cd(i*5+j+1);
      hHPeak[i][j]->Draw();
    }
  }
  TCanvas *cLAmpl = new TCanvas("cLAmpl", "Low Gain Amplitude", 800, 600);
  cLAmpl->Divide(5,5);
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cLAmpl->cd(i*5+j+1);
      hLAmpl[i][j]->Draw();
    }
  }
  TCanvas *cHAmpl = new TCanvas("cHAmpl", "High Gain Amplitude", 800, 600);
  cHAmpl->Divide(5,5); 
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      cHAmpl->cd(i*5+j+1);
      hHAmpl[i][j]->Draw();
    }
  }
  TCanvas *cLNoise = new TCanvas("cLNoise", "Low Gain Noise", 800, 600);
  cLNoise->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cLNoise->cd(i*5+j+1);
      hLNoise[i][j]->Draw();
    }
  }
  TCanvas *cHNoise = new TCanvas("cHNoise", "High Gain Noise", 800, 600);
  cHNoise->Divide(5,5);
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cHNoise->cd(i*5+j+1);
      hHNoise[i][j]->Draw();
    }
  }
  TCanvas *cLAmplTime = new TCanvas("cLAmplTime", "Low Gain Amplitude vs Time", 800, 600);
  cLAmplTime->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cLAmplTime->cd(i*5+j+1);
      gLAmplTime[i][j]->SetTitle(Form("Low Gain Amplitude vs Time Crystal (%d,%d);Time(ns);Low Gain Amplitude", i+1, j+1));
      gLAmplTime[i][j]->Draw("AP"); 
    }
  }
  TCanvas *cHAmplTime = new TCanvas("cHAmplTime", "High Gain Amplitude vs Time", 800, 600);
  cHAmplTime->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cHAmplTime->cd(i*5+j+1);
      gHAmplTime[i][j]->SetTitle(Form("High Gain Amplitude vs Time Crystal (%d,%d);Time(ns);High Gain Amplitude", i+1, j+1));
      gHAmplTime[i][j]->Draw("AP");
    }
  }
  TCanvas *cLNoiseTime = new TCanvas("cLNoiseTime", "Low Gain Noise vs Time", 800, 600);
  cLNoiseTime->Divide(5,5); 
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cLNoiseTime->cd(i*5+j+1);
      gLNoiseTime[i][j]->SetTitle(Form("Low Gain Noise vs Time Crystal (%d,%d);Time(ns);Low Gain Noise", i+1, j+1));
      gLNoiseTime[i][j]->Draw("AP"); 
    }
  } 
  TCanvas *cHNoiseTime = new TCanvas("cHNoiseTime", "High Gain Noise vs Time", 800, 600);
  cHNoiseTime->Divide(5,5);
  for(Int_t i=0; i<5; i++){ 
    for(Int_t j=0; j<5; j++){
      cHNoiseTime->cd(i*5+j+1);
      gHNoiseTime[i][j]->SetTitle(Form("High Gain Noise vs Time Crystal (%d,%d);Time(ns);High Gain Noise", i+1, j+1));
      gHNoiseTime[i][j]->Draw("AP");
    }
  }

  TFile *fout = new TFile(rootname, "RECREATE");
  fout->cd();
  cTemp1->Write();
  cTemp2->Write();
  cLPed->Write();
  cHPed->Write();
  cLPeak->Write();
  cHPeak->Write();
  cLAmpl->Write();
  cHAmpl->Write();
  cLNoise->Write();
  cHNoise->Write();
  cLAmplTime->Write();
  cHAmplTime->Write();
  cLNoiseTime->Write();
  cHNoiseTime->Write();
  cTime->Write();
  cEventID->Write();
  cTriggerID->Write();
  fout->Close();
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv<2)
   {
      cout<<"Usage: ./drawval rootfile"<<endl; 
      return 1;
   }
    TString rootname=argc[1];
    drawval(rootname);
    return 0;
}
#endif

