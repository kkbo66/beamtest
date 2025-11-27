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
// 画出5x5通道累加的（前1000事例）波形和噪声
void Draw5x5SumWave(TString rootname)
{

  // gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTitleFont(42, "xyz");
  gStyle->SetLabelFont(42, "xyz");
  gStyle->SetLegendFont(42);
  gStyle->SetTextFont(42);
  gStyle->SetTitleSize(0.05, "xyz");
  gStyle->SetLabelSize(0.04, "xyz");
  gStyle->SetLegendTextSize(0.04);
  gStyle->SetTitleOffset(1.2, "y");
  gStyle->SetTitleOffset(1.0, "x");

  size_t dotpos = rootname.Last('.');
  size_t slashpos = rootname.Last('/');
  TString filename = rootname(slashpos + 1, dotpos - slashpos - 1);
  cout << "File name: " << filename << endl;
  TFile *f1 = new TFile(rootname);
  TTree *t1 = (TTree *)f1->Get("decode_data");
  Int_t nentries = (Int_t)t1->GetEntries();

  // const Int_t nPoints = 256;
  TGraph *gLAmplTime[5][5];
  TGraph *gHAmplTime[5][5];
  TGraph *gLNoiseTime[5][5];
  TGraph *gHNoiseTime[5][5];
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      gLAmplTime[i][j] = new TGraph();
      gHAmplTime[i][j] = new TGraph();
      gLNoiseTime[i][j] = new TGraph();
      gHNoiseTime[i][j] = new TGraph();
      gLAmplTime[i][j]->SetName(Form("LGAmplitude_Hit_%d_%d", i + 1, j + 1));
      gHAmplTime[i][j]->SetName(Form("HGAmplitude_Hit_%d_%d", i + 1, j + 1));
      gLNoiseTime[i][j]->SetName(Form("LGNoise_Hit_%d_%d", i + 1, j + 1));
      gHNoiseTime[i][j]->SetName(Form("HGNoise_Hit_%d_%d", i + 1, j + 1));
    }
  }

  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      TTreeReader myReader(t1);
      TTreeReaderArray<Double_t> LAmplitude(myReader, Form("Hit_%d_%d.LAmplitude", i + 1, j + 1));
      TTreeReaderArray<Double_t> HAmplitude(myReader, Form("Hit_%d_%d.HAmplitude", i + 1, j + 1));
      TTreeReaderArray<Double_t> LNoise(myReader, Form("Hit_%d_%d.LNoise", i + 1, j + 1));
      TTreeReaderArray<Double_t> HNoise(myReader, Form("Hit_%d_%d.HNoise", i + 1, j + 1));
      int nEntries = 0;
      while (myReader.Next())
      {
        nEntries++;
        if (nEntries > 1000)
          break; // only draw first 5000 events
        for (Int_t k = 0; k < LAmplitude.GetSize(); k++)
        {
          gLAmplTime[i][j]->SetPoint(gLAmplTime[i][j]->GetN(), k * 12.5, LAmplitude[k]);
        }
        for (Int_t k = 0; k < HAmplitude.GetSize(); k++)
        {
          gHAmplTime[i][j]->SetPoint(gHAmplTime[i][j]->GetN(), k * 12.5, HAmplitude[k]);
        }
        for (Int_t k = 0; k < LNoise.GetSize(); k++)
        {
          gLNoiseTime[i][j]->SetPoint(gLNoiseTime[i][j]->GetN(), k * 12.5, LNoise[k]);
        }
        for (Int_t k = 0; k < HNoise.GetSize(); k++)
        {
          gHNoiseTime[i][j]->SetPoint(gHNoiseTime[i][j]->GetN(), k * 12.5, HNoise[k]);
        }
      }
    }
  }

  TCanvas *cLAmplTime = new TCanvas("cLAmplTime", "Low Gain Amplitude vs Time", 800, 600);
  cLAmplTime->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      // cLAmplTime->cd(i*5+j+1);
      cLAmplTime->cd(21 + i - 5 * j);
      gLAmplTime[i][j]->SetTitle(Form("Low Gain Amplitude vs Time Crystal (%d,%d);Time(ns);Low Gain Amplitude", i + 1, j + 1));
      gLAmplTime[i][j]->Draw("AP");
    }
  }
  TCanvas *cHAmplTime = new TCanvas("cHAmplTime", "High Gain Amplitude vs Time", 800, 600);
  cHAmplTime->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      // cHAmplTime->cd(i*5+j+1);
      cHAmplTime->cd(21 + i - 5 * j);
      gHAmplTime[i][j]->SetTitle(Form("High Gain Amplitude vs Time Crystal (%d,%d);Time(ns);High Gain Amplitude", i + 1, j + 1));
      gHAmplTime[i][j]->Draw("AP");
    }
  }
  TCanvas *cLNoiseTime = new TCanvas("cLNoiseTime", "Low Gain Noise vs Time", 800, 600);
  cLNoiseTime->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      // cLNoiseTime->cd(i*5+j+1);
      cLNoiseTime->cd(21 + i - 5 * j);
      gLNoiseTime[i][j]->SetTitle(Form("Low Gain Noise vs Time Crystal (%d,%d);Time(ns);Low Gain Noise", i + 1, j + 1));
      gLNoiseTime[i][j]->Draw("AP");
    }
  }
  TCanvas *cHNoiseTime = new TCanvas("cHNoiseTime", "High Gain Noise vs Time", 800, 600);
  cHNoiseTime->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      // cHNoiseTime->cd(i*5+j+1);
      cHNoiseTime->cd(21 + i - 5 * j);
      gHNoiseTime[i][j]->SetTitle(Form("High Gain Noise vs Time Crystal (%d,%d);Time(ns);High Gain Noise", i + 1, j + 1));
      gHNoiseTime[i][j]->Draw("AP");
    }
  }

  TFile *fout = new TFile("./5x5sumwave-" + filename + ".root", "RECREATE");
  fout->cd();
  cLAmplTime->Write();
  cHAmplTime->Write();
  cLNoiseTime->Write();
  cHNoiseTime->Write();
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++)
    {
      gHAmplTime[i][j]->Write();
      gLAmplTime[i][j]->Write();
      gHNoiseTime[i][j]->Write();
      gLNoiseTime[i][j]->Write();
    }
  fout->Close();
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv, char *argc[])
{
  if (argv < 2)
  {
    cout << "Usage: ./drawsumwave rootfile" << endl;
    return 1;
  }
  TString rootname = argc[1];
  Draw5x5SumWave(rootname);
  return 0;
}
#endif
