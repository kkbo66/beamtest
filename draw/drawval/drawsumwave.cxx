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

void drawsumwave(TString rootname){

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

  size_t dotpos = rootname.Last('.');
  size_t slashpos = rootname.Last('/');
  TString filename = rootname(slashpos+1, dotpos-slashpos-1);
  cout<<"filename: "<<filename<<endl;
  TFile *f1 = new TFile(rootname);
  //TFile *f1 = new TFile("/home/kkbo/beamtest/2025/ECAL/build/"+rootname);
  //TFile *f1 = new TFile("/home/kkbo/beamtest/2025/ECAL/rootfile/"+rootname);
  TTree *t1 = (TTree*)f1->Get("decode_data");
  Int_t nentries = (Int_t)t1->GetEntries();

  //const Int_t nPoints = 256;
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
 
  double numunderflow[5][5] = {0};

  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      TTreeReader myReader(t1);
      TTreeReaderArray<Double_t> LAmplitude(myReader, Form("Hit_%d_%d.LAmplitude", i+1, j+1));
      TTreeReaderArray<Double_t> HAmplitude(myReader, Form("Hit_%d_%d.HAmplitude", i+1, j+1));
      TTreeReaderArray<Double_t> LNoise(myReader, Form("Hit_%d_%d.LNoise", i+1, j+1));
      TTreeReaderArray<Double_t> HNoise(myReader, Form("Hit_%d_%d.HNoise", i+1, j+1));
      while (myReader.Next()) {
        for(Int_t k=0; k<LAmplitude.GetSize(); k++){
          gLAmplTime[i][j]->SetPoint(gLAmplTime[i][j]->GetN(), k*12.5, LAmplitude[k]);
        }
        for(Int_t k=0; k<HAmplitude.GetSize(); k++){
          gHAmplTime[i][j]->SetPoint(gHAmplTime[i][j]->GetN(), k*12.5, HAmplitude[k]);
          if(HAmplitude[k]<2100) numunderflow[i][j]++;
        }
        for(Int_t k=0; k<LNoise.GetSize(); k++){
          gLNoiseTime[i][j]->SetPoint(gLNoiseTime[i][j]->GetN(), k*12.5, LNoise[k]);
        }
        for(Int_t k=0; k<HNoise.GetSize(); k++){
          gHNoiseTime[i][j]->SetPoint(gHNoiseTime[i][j]->GetN(), k*12.5, HNoise[k]);
        }
      }
    }
  }

  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      cout<<"Crystal ("<<i+1<<","<<j+1<<") underflow number: "<<numunderflow[i][j]<<endl;
    }
  }

  //TCanvas *cLAmplTime = new TCanvas("cLAmplTime", "Low Gain Amplitude vs Time", 800, 600);
  //cLAmplTime->Divide(5,5); 
  //for(Int_t i=0; i<5; i++){ 
  //  for(Int_t j=0; j<5; j++){
  //    cLAmplTime->cd(i*5+j+1);
  //    gLAmplTime[i][j]->SetTitle(Form("Low Gain Amplitude vs Time Crystal (%d,%d);Time(ns);Low Gain Amplitude", i+1, j+1));
  //    gLAmplTime[i][j]->Draw("AP"); 
  //  }
  //}
  //TCanvas *cHAmplTime = new TCanvas("cHAmplTime", "High Gain Amplitude vs Time", 800, 600);
  //cHAmplTime->Divide(5,5); 
  //for(Int_t i=0; i<5; i++){ 
  //  for(Int_t j=0; j<5; j++){
  //    cHAmplTime->cd(i*5+j+1);
  //    gHAmplTime[i][j]->SetTitle(Form("High Gain Amplitude vs Time Crystal (%d,%d);Time(ns);High Gain Amplitude", i+1, j+1));
  //    gHAmplTime[i][j]->Draw("AP");
  //  }
  //}
  //TCanvas *cLNoiseTime = new TCanvas("cLNoiseTime", "Low Gain Noise vs Time", 800, 600);
  //cLNoiseTime->Divide(5,5); 
  //for(Int_t i=0; i<5; i++){ 
  //  for(Int_t j=0; j<5; j++){
  //    cLNoiseTime->cd(i*5+j+1);
  //    gLNoiseTime[i][j]->SetTitle(Form("Low Gain Noise vs Time Crystal (%d,%d);Time(ns);Low Gain Noise", i+1, j+1));
  //    gLNoiseTime[i][j]->Draw("AP"); 
  //  }
  //} 
  //TCanvas *cHNoiseTime = new TCanvas("cHNoiseTime", "High Gain Noise vs Time", 800, 600);
  //cHNoiseTime->Divide(5,5);
  //for(Int_t i=0; i<5; i++){ 
  //  for(Int_t j=0; j<5; j++){
  //    cHNoiseTime->cd(i*5+j+1);
  //    gHNoiseTime[i][j]->SetTitle(Form("High Gain Noise vs Time Crystal (%d,%d);Time(ns);High Gain Noise", i+1, j+1));
  //    gHNoiseTime[i][j]->Draw("AP");
  //  }
  //}

  TCanvas *subLAmplTime[5][5];
  TCanvas *subHAmplTime[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      subLAmplTime[i][j] = new TCanvas(Form("subLAmplTime_%d_%d", i+1, j+1), Form("Low Gain Amplitude vs Time Crystal (%d,%d)", i+1, j+1), 800, 600);
      gLAmplTime[i][j]->SetTitle(Form("Low Gain Amplitude vs Time Crystal (%d,%d);Time(ns);Low Gain Amplitude", i+1, j+1));
      gLAmplTime[i][j]->Draw("AP");
      subHAmplTime[i][j] = new TCanvas(Form("subHAmplTime_%d_%d", i+1, j+1), Form("High Gain Amplitude vs Time Crystal (%d,%d)", i+1, j+1), 800, 600);
      gHAmplTime[i][j]->SetTitle(Form("High Gain Amplitude vs Time Crystal (%d,%d);Time(ns);High Gain Amplitude", i+1, j+1));
      gHAmplTime[i][j]->Draw("AP");
    }
  }
  TCanvas *subLNoiseTime[5][5];
  TCanvas *subHNoiseTime[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      subLNoiseTime[i][j] = new TCanvas(Form("subLNoiseTime_%d_%d", i+1, j+1), Form("Low Gain Noise vs Time Crystal (%d,%d)", i+1, j+1), 800, 600);
      gLNoiseTime[i][j]->SetTitle(Form("Low Gain Noise vs Time Crystal (%d,%d);Time(ns);Low Gain Noise", i+1, j+1));
      gLNoiseTime[i][j]->Draw("AP");
      subHNoiseTime[i][j] = new TCanvas(Form("subHNoiseTime_%d_%d", i+1, j+1), Form("High Gain Noise vs Time Crystal (%d,%d)", i+1, j+1), 800, 600);    
      gHNoiseTime[i][j]->SetTitle(Form("High Gain Noise vs Time Crystal (%d,%d);Time(ns);High Gain Noise", i+1, j+1));
      gHNoiseTime[i][j]->Draw("AP");
    }
  } 

  TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/sumwave-"+filename+".root", "RECREATE");
  fout->cd();
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      //gLAmplTime[i][j]->Write(Form("gLAmplTime_%d_%d", i+1, j+1));
      //gHAmplTime[i][j]->Write(Form("gHAmplTime_%d_%d", i+1, j+1));
      //gLNoiseTime[i][j]->Write(Form("gLNoiseTime_%d_%d", i+1, j+1));
      //gHNoiseTime[i][j]->Write(Form("gHNoiseTime_%d_%d", i+1, j+1));
      subLNoiseTime[i][j]->Write();
      subHNoiseTime[i][j]->Write();
      subLAmplTime[i][j]->Write();
      subHAmplTime[i][j]->Write();
    }
  }
  fout->Close();

}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv<2)
   {
      cout<<"Usage: ./drawsumwave rootfile"<<endl; 
      return 1;
   }
    TString rootname=argc[1];
    drawsumwave(rootname);
    return 0;
}
#endif

