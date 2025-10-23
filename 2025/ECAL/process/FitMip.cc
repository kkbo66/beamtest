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
#include <TF1.h>
#include <TMath.h>
#include <TString.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TChain.h>

using namespace std;
using namespace TMath;

double langaufun(Double_t *x, Double_t *par)
{

  // Fit parameters:
  // par[0]=Width (scale) parameter of Landau density
  // par[1]=Most Probable (MP, location) parameter of Landau density
  // par[2]=Total area (integral -inf to inf, normalization constant)
  // par[3]=Width (sigma) of convoluted Gaussian function
  //
  // In the Landau distribution (represented by the CERNLIB approximation),
  // the maximum is located at x=-0.22278298 with the location parameter=0.
  // This shift is corrected within this function, so that the actual
  // maximum is identical to the MP parameter.

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014; // (2 pi)^(-1/2)
  Double_t mpshift = -0.22278298;      // Landau maximum location

  // Control constants
  Double_t np = 100.0; // number of convolution steps
  Double_t sc = 5.0;   // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow, xupp;
  Double_t step;
  Double_t i;

  // MP shift correction
  mpc = par[1] - mpshift * par[0];

  // Range of convolution integral
  xlow = x[0] - sc * par[3];
  xupp = x[0] + sc * par[3];

  step = (xupp - xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for (i = 1.0; i <= np / 2; i++)
  {
    xx = xlow + (i - .5) * step;
    fland = TMath::Landau(xx, mpc, par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0], xx, par[3]);

    xx = xupp - (i - .5) * step;
    fland = TMath::Landau(xx, mpc, par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0], xx, par[3]);
  }

  return (par[2] * step * sum * invsq2pi / par[3]);
}

void getMIP(vector<TString> rootfiles, double cut)
{

  TChain *chain = new TChain("decode_data");
  for (auto &file : rootfiles)
  {
    chain->Add(file);
    cout << "Added file: " << file << endl;
  }
  int nEntries = chain->GetEntries();
  cout << "Total entries in chain: " << nEntries << endl;

  vector<vector<Double_t>> vLowGainPedestal;
  vector<vector<Double_t>> vHighGainPedestal;
  vector<vector<Double_t>> vLowGainPeak;
  vector<vector<Double_t>> vHighGainPeak;
  vLowGainPedestal.clear();
  vHighGainPedestal.clear();
  vLowGainPeak.clear();
  vHighGainPeak.clear();

  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      cout << "Processing Channel " << i + 1 << "_" << j + 1 << " ... " << flush << endl;
      TTreeReader myReader(chain);
      TTreeReaderValue<Double_t> LowGainPedestal(myReader, Form("Hit_%d_%d.LowGainPedestal", i + 1, j + 1));
      TTreeReaderValue<Double_t> HighGainPedestal(myReader, Form("Hit_%d_%d.HighGainPedestal", i + 1, j + 1));
      TTreeReaderArray<Double_t> LAmplitude(myReader, Form("Hit_%d_%d.LAmplitude", i + 1, j + 1));
      TTreeReaderArray<Double_t> HAmplitude(myReader, Form("Hit_%d_%d.HAmplitude", i + 1, j + 1));
      vector<Double_t> vLPed;
      vector<Double_t> vHPed;
      vector<Double_t> vLPeak;
      vector<Double_t> vHPeak;
      vLPed.clear();
      vHPed.clear();
      vLPeak.clear();
      vHPeak.clear();
      while (myReader.Next())
      {
        vLPed.push_back(*LowGainPedestal);
        vHPed.push_back(*HighGainPedestal);
        Double_t LPeak = -1;
        Double_t HPeak = -1;
        for (size_t k = 0; k < LAmplitude.GetSize(); k++)
        {
          if (LAmplitude[k] > LPeak)
            LPeak = LAmplitude[k];
        }
        for (size_t k = 0; k < HAmplitude.GetSize(); k++)
        {
          if (HAmplitude[k] > HPeak)
            HPeak = HAmplitude[k];
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

  TFile *fout = new TFile("MIP_Spectra.root", "RECREATE");
  TTree *tree = new TTree("MIP_Spectra", "MIP_Spectra");
  tree->SetDirectory(fout);
  double vLowGainMIP[5][5];
  double vHighGainMIP[5][5];
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      tree->Branch(Form("LowGainMIP_%d_%d", i + 1, j + 1), &vLowGainMIP[i][j], Form("LowGainMIP_%d_%d/D", i + 1, j + 1));
      tree->Branch(Form("HighGainMIP_%d_%d", i + 1, j + 1), &vHighGainMIP[i][j], Form("HighGainMIP_%d_%d/D", i + 1, j + 1));
    }
  }
  for (size_t entry = 0; entry < nEntries; entry++)
  {
    int bitmask = 0;
    for (Int_t i = 0; i < 5; i++)
    {
      for (Int_t j = 0; j < 5; j++)
      {
        Double_t LowGainMIP = vLowGainPeak[5 * i + j][entry] - vLowGainPedestal[5 * i + j][entry];
        Double_t HighGainMIP = vHighGainPeak[5 * i + j][entry] - vHighGainPedestal[5 * i + j][entry];
        if (HighGainMIP > cut)
        {
          bitmask += 1;
          vHighGainMIP[i][j] = HighGainMIP;
          vLowGainMIP[i][j] = LowGainMIP;
        }
        else
        {
          vHighGainMIP[i][j] = -1;
          vLowGainMIP[i][j] = -1;
        }
      }
    }
    // 只挑选单通道击中的MIP事例
    if (bitmask != 1)
      continue;
    tree->Fill();
  }
  fout->cd();
  tree->Write();
  fout->Close();
}

void fitmip()
{
  TFile *fin = new TFile("./MIP_Spectra.root", "READ");
  TTree *tree = (TTree *)fin->Get("MIP_Spectra");

  double vLowGainMIP[5][5];
  double vHighGainMIP[5][5];
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      tree->SetBranchAddress(Form("LowGainMIP_%d_%d", i + 1, j + 1), &vLowGainMIP[i][j]);
      tree->SetBranchAddress(Form("HighGainMIP_%d_%d", i + 1, j + 1), &vHighGainMIP[i][j]);
    }
  }

  TH1F *hLowGainMIP[5][5];
  TH1F *hHighGainMIP[5][5];
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      hLowGainMIP[i][j] = new TH1F(Form("hLowGainMIP_%d_%d", i + 1, j + 1), Form("Low Gain MIP Spectrum %d %d", i + 1, j + 1), 100, 0, 1300);
      hHighGainMIP[i][j] = new TH1F(Form("hHighGainMIP_%d_%d", i + 1, j + 1), Form("High Gain MIP Spectrum %d %d", i + 1, j + 1), 100, 0, 13000);
    }
  }
  for (Int_t entry = 0; entry < tree->GetEntries(); entry++)
  {
    tree->GetEntry(entry);
    for (Int_t i = 0; i < 5; i++)
    {
      for (Int_t j = 0; j < 5; j++)
      {
        hLowGainMIP[i][j]->Fill(vLowGainMIP[i][j]);
        hHighGainMIP[i][j]->Fill(vHighGainMIP[i][j]);
      }
    }
  }

  ofstream entryfile("entries.txt");
  ofstream lowfile("./parameters_mipfit_lowgain.txt");
  ofstream lowpeakfile("./LGMipPeak.dat");
  TCanvas *c1 = new TCanvas("c1", "MIP Fit Low Gain", 800, 600);
  c1->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      c1->cd(21 + i - 5 * j);
      hLowGainMIP[i][j]->Draw();
      entryfile << Form("Low Gain MIP %d %d entries: ", i + 1, j + 1)
                << hLowGainMIP[i][j]->Integral() << endl;
      int maxbin = hLowGainMIP[i][j]->GetMaximumBin();
      double maxvalue = hLowGainMIP[i][j]->GetBinCenter(maxbin);
      TF1 *fitLow = new TF1("fitLow", langaufun, maxvalue - 200, maxvalue + 500, 4);
      fitLow->SetParameters(maxvalue, 10, 50, 1);
      TFitResultPtr r = hLowGainMIP[i][j]->Fit(fitLow, "RS");
      // for (int iter = 0; iter < 50; iter++)
      // {
      //   if (r->IsValid() && r->Status() == 0)
      //     break;
      //   else
      //   {
      //     fitLow->SetParameters(maxvalue * (0.99 + 0.002 * (iter + 1)), 10 + 10 * (iter + 1), 50 + 10 * (iter + 1), 1);
      //     r = hLowGainMIP[i][j]->Fit(fitLow, "RS");
      //   }
      // }
      fitLow->Draw("same");
      lowfile << Form("Low Gain MIP %d %d: ", i + 1, j + 1)
              << "MPV = " << fitLow->GetParameter(0) << ", "
              << "LandauWidth = " << fitLow->GetParameter(1) << ", "
              << "GaussSigma = " << fitLow->GetParameter(2) << endl;
      lowpeakfile << fitLow->GetMaximumX() << endl;
    }
  }
  lowfile.close();
  lowpeakfile.close();
  ofstream highfile("./parameters_mipfit_highgain.txt");
  ofstream highpeakfile("./HGMipPeak.dat");
  TCanvas *c2 = new TCanvas("c2", "MIP Fit High Gain", 800, 600);
  c2->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      c2->cd(21 + i - 5 * j);
      hHighGainMIP[i][j]->Draw();
      entryfile << Form("High Gain MIP %d %d entries: ", i + 1, j + 1)
                << hHighGainMIP[i][j]->Integral() << endl;
      int maxbin = hHighGainMIP[i][j]->GetMaximumBin();
      double maxvalue = hHighGainMIP[i][j]->GetBinCenter(maxbin);
      TF1 *fitHigh = new TF1("fitHigh", langaufun, maxvalue - 2000, maxvalue + 5000, 4);
      fitHigh->SetParameters(maxvalue, 100, 600, 1);
      TFitResultPtr r = hHighGainMIP[i][j]->Fit(fitHigh, "RS");
      // for (int iter = 0; iter < 50; iter++)
      // {
      //   if (r->IsValid() && r->Status() == 0)
      //     break;
      //   else
      //   {
      //     fitHigh->SetParameters(maxvalue * (0.99 + 0.002 * (iter + 1)), 100 + 100 * (iter + 1), 500 + 100 * (iter + 1), 1);
      //     r = hHighGainMIP[i][j]->Fit(fitHigh, "RS");
      //   }
      // }
      fitHigh->Draw("same");
      highfile << Form("High Gain MIP %d %d: ", i + 1, j + 1)
               << "MPV = " << fitHigh->GetParameter(0) << ", "
               << "LandauWidth = " << fitHigh->GetParameter(1) << ", "
               << "GaussSigma = " << fitHigh->GetParameter(2) << endl;
      highpeakfile << fitHigh->GetMaximumX() << endl;
    }
  }

  TFile *fout = new TFile("./MIP_Fit_Results.root", "RECREATE");
  fout->cd();
  c1->Write();
  c2->Write();
  fout->Close();
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv, char *argc[])
{
  if (argv < 2)
  {
    cout << "Please input root file or (rootlist).txt or more cutvalue" << endl;
    return 1;
  }
  for (Int_t i = 0; i < argv; i++)
  {
    cout << argc[i] << endl;
  }
  vector<TString> rootfiles;
  rootfiles.clear();
  if (string(argc[1]).find(".txt") != string::npos)
  {
    ifstream infile;
    infile.open(argc[1], ios::in);
    string line;
    while (getline(infile, line))
    {
      TString rootname = line;
      rootfiles.push_back(rootname);
    }
    infile.close();
  }
  else if (string(argc[1]).find(".root") == string::npos)
  {
    cout << "Please input root file or (rootlist).txt" << endl;
    return 1;
  }
  else
  {
    TString rootname = argc[1];
    rootfiles.push_back(rootname);
  }
  TString rootname = argc[1];
  if (argv == 3)
  {
    double cutvalue = stod(argc[2]);
    getMIP(rootfiles, cutvalue);
  }
  else
    getMIP(rootfiles, 5000);
  fitmip();
  return 0;
}
#endif