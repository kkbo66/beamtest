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
#include <TH2F.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLine.h>
#include <TLatex.h>
#include <TF1.h>
#include <TMath.h>
#include <TString.h>
#include <TAxis.h>
#include <TPaveStats.h>
#include "DataModel2025.hh"
using namespace std;
using namespace TMath;

double fratio(double *x, double *par)
{
  if (x[0] < par[2])
    return par[0] * x[0] + par[1];
  else
    return par[0] * par[2] + par[1];
}
void Draw5x5NoiseTempGratio(TString rootname)
{

  gStyle->SetOptStat(0000);
  // gStyle->SetOptFit(1110);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetStatX(0.7);
  gStyle->SetStatY(0.3);
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
  gStyle->SetTitleOffset(0.8, "x");
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);

  size_t dotpos = rootname.Last('.');
  size_t slashpos = rootname.Last('/');
  TString filename = rootname(slashpos + 1, dotpos - slashpos - 1);
  TFile *infile = new TFile(rootname, "READ");
  TTree *tr = (TTree *)infile->Get("decode_data");
  Int_t Nentries = (Int_t)tr->GetEntries();

  double HGpedestal[25], HGnoise[25], LGpedestal[25], LGnoise[25], temperature1[25], temperature2[25], ratio[25];
  TLatex *tex = new TLatex();
  tex->SetTextFont(42);
  tex->SetTextAlign(20);
  tex->SetTextSize(0.1);
  tex->SetTextColor(kBlue);

  // const Int_t nPoints = 256;
  TH1F *hTemp1[5][5];
  TH1F *hTemp2[5][5];
  TH1F *hLNoise[5][5];
  TH1F *hHNoise[5][5];
  TGraph *gHLratio[5][5];
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++)
    {
      hTemp1[i][j] = new TH1F(Form("temp1_%d_%d", i + 1, j + 1), Form("temp1_%d_%d;tempreture/{}^{o}C;counts", i + 1, j + 1), 1000, 0, 100);
      hTemp2[i][j] = new TH1F(Form("temp2_%d_%d", i + 1, j + 1), Form("temp2_%d_%d;tempreture/{}^{o}C;counts", i + 1, j + 1), 1000, 0, 100);
      hLNoise[i][j] = new TH1F(Form("LGnoise_%d_%d", i + 1, j + 1), Form("LGnoise_%d_%d;ADC value;counts", i + 1, j + 1), 1000, 2000, 3000);
      hHNoise[i][j] = new TH1F(Form("HGnoise_%d_%d", i + 1, j + 1), Form("HGnoise_%d_%d;ADC value;counts", i + 1, j + 1), 1000, 2000, 3000);
      gHLratio[i][j] = new TGraph();
      gHLratio[i][j]->SetNameTitle(Form("GainRatio_%d_%d", i + 1, j + 1), Form("GainRatio_%d_%d;Low Gain Peak;High Gain Peak", i + 1, j + 1));
    }

  DataModel2025 hit[25];

  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++)
    {
      tr->SetBranchAddress(Form("Hit_%d_%d", i + 1, j + 1), &hit[5 * i + j]);
    }
  int process;
  for (int n = 0; n < Nentries; n++)
  {
    if (n % (Nentries / 100) == 0 && n != 0)
    {
      process = 100 * n / (Nentries);
      std::cout << "\rProcessing: >>>>>>>>>>>>>>> " << process << "% <<<<<<<<<<<<<<<";
      std::cout.flush();
    }
    tr->GetEntry(n);
    for (int i = 0; i < 5; i++)
      for (int j = 0; j < 5; j++)
      {
        int ch = 5 * i + j;
        hTemp1[i][j]->Fill(hit[ch].Temperature1);
        hTemp2[i][j]->Fill(hit[ch].Temperature2);
        for (int k = 0; k < _Nnoise; k++)
        {
          hLNoise[i][j]->Fill(hit[ch].LNoise[k]);
          hHNoise[i][j]->Fill(hit[ch].HNoise[k]);
        }
        gHLratio[i][j]->SetPoint(gHLratio[i][j]->GetN(), hit[ch].LowGainPeak - hit[ch].LowGainPedestal, hit[ch].HighGainPeak - hit[ch].HighGainPedestal);
      }
  }
  std::cout << std::endl;

  TCanvas *cTemp1 = new TCanvas("cTemp1", "Temperature1", 1600, 900);
  cTemp1->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      cTemp1->cd(21 + i - 5 * j);
      hTemp1[i][j]->GetXaxis()->SetRangeUser(hTemp1[i][j]->GetMean() - 5 * hTemp1[i][j]->GetRMS(), hTemp1[i][j]->GetMean() + 5 * hTemp1[i][j]->GetRMS());
      hTemp1[i][j]->Draw();
      temperature1[5 * i + j] = hTemp1[i][j]->GetMean();
    }
  }

  TCanvas *cTemp2 = new TCanvas("cTemp2", "Temperature2", 1600, 900);
  cTemp2->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      cTemp2->cd(21 + i - 5 * j);
      hTemp2[i][j]->GetXaxis()->SetRangeUser(hTemp2[i][j]->GetMean() - 5 * hTemp2[i][j]->GetRMS(), hTemp2[i][j]->GetMean() + 5 * hTemp2[i][j]->GetRMS());
      hTemp2[i][j]->Draw();
      temperature1[5 * i + j] = hTemp1[i][j]->GetMean();
    }
  }

  TH2F *his_LGnoise = new TH2F("hisLGnoise", ";row;column;sigma", 5, 0, 5, 5, 0, 5);
  his_LGnoise->SetDirectory(nullptr);
  TCanvas *cLNoise = new TCanvas("cLNoise", "Low Gain Noise", 1600, 900);
  cLNoise->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      cLNoise->cd(21 + i - 5 * j);
      hLNoise[i][j]->GetXaxis()->SetRangeUser(hLNoise[i][j]->GetMean() - 5 * hLNoise[i][j]->GetRMS(), hLNoise[i][j]->GetMean() + 5 * hLNoise[i][j]->GetRMS());
      hLNoise[i][j]->Draw();
      hLNoise[i][j]->Fit("gaus", "Q");
      LGpedestal[5 * i + j] = hLNoise[i][j]->GetFunction("gaus")->GetParameter(1);
      LGnoise[5 * i + j] = hLNoise[i][j]->GetFunction("gaus")->GetParameter(2);
      his_LGnoise->Fill(i, j, hLNoise[i][j]->GetFunction("gaus")->GetParameter(2));
      gPad->Update();
      tex->DrawLatexNDC(0.5, 0.5, Form("Mean=%.2lf", LGpedestal[5 * i + j]));
      tex->DrawLatexNDC(0.5, 0.35, Form("Sigma=%.2lf", LGnoise[5 * i + j]));
      TPaveStats *ptstats = (TPaveStats *)hLNoise[i][j]->GetListOfFunctions()->FindObject("stats");
      if (ptstats)
      {
        ptstats->SetX1NDC(0.6);
        ptstats->SetX2NDC(0.95);
        ptstats->SetY1NDC(0.6);
        ptstats->SetY2NDC(0.9);
      }
      gPad->Modified();
      gPad->Update();
    }
  }
  TCanvas *cLNoisedis = new TCanvas("cLNoisedis", "Low Gain Noise", 1600, 900);
  gPad->SetGrid(0, 0);
  his_LGnoise->Draw("colz");
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++)
      tex->DrawLatexNDC(0.15 * (i + 1) + 0.08, 0.16 * (j + 1), Form("%.2lf", LGnoise[5 * i + j]));
  cLNoisedis->SaveAs("./LGnoisedis.pdf");

  TH2F *his_HGnoise = new TH2F("hisHGnoise", ";row;column;sigma", 5, 0, 5, 5, 0, 5);
  his_HGnoise->SetDirectory(nullptr);
  TCanvas *cHNoise = new TCanvas("cHNoise", "High Gain Noise", 1600, 900);
  cHNoise->Divide(5, 5);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      cHNoise->cd(21 + i - 5 * j);
      hHNoise[i][j]->GetXaxis()->SetRangeUser(hHNoise[i][j]->GetMean() - 5 * hHNoise[i][j]->GetRMS(), hHNoise[i][j]->GetMean() + 5 * hHNoise[i][j]->GetRMS());
      hHNoise[i][j]->Draw();
      hHNoise[i][j]->Fit("gaus", "Q");
      HGpedestal[5 * i + j] = hHNoise[i][j]->GetFunction("gaus")->GetParameter(1);
      HGnoise[5 * i + j] = hHNoise[i][j]->GetFunction("gaus")->GetParameter(2);
      his_HGnoise->Fill(i, j, hHNoise[i][j]->GetFunction("gaus")->GetParameter(2));
      tex->DrawLatexNDC(0.5, 0.5, Form("Mean=%.2lf", HGpedestal[5 * i + j]));
      tex->DrawLatexNDC(0.5, 0.35, Form("Sigma=%.2lf", HGnoise[5 * i + j]));
      TPaveStats *ptstats = (TPaveStats *)hHNoise[i][j]->GetListOfFunctions()->FindObject("stats");
      if (ptstats)
      {
        ptstats->SetX1NDC(0.6);
        ptstats->SetX2NDC(0.95);
        ptstats->SetY1NDC(0.6);
        ptstats->SetY2NDC(0.9);
      }
      gPad->Modified();
      gPad->Update();
    }
  }
  // cHNoise->SaveAs("./HGnoise.pdf");
  TCanvas *cHNoisedis = new TCanvas("cHNoisedis", "High Gain Noise", 1600, 900);
  gPad->SetGrid(0, 0);
  his_HGnoise->Draw("colz");
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++)
      tex->DrawLatexNDC(0.15 * (i + 1) + 0.08, 0.16 * (j + 1), Form("%.2lf", HGnoise[5 * i + j]));
  cHNoisedis->SaveAs("./HGnoisedis.pdf");

  TCanvas *cHLratio = new TCanvas("HLGainRatio", "Gain ratio", 1600, 900);
  cHLratio->Divide(5, 5);
  TF1 *fHGratio = new TF1("fratio", fratio, 4, 16384, 3);
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      cHLratio->cd(21 + i - 5 * j);
      gHLratio[i][j]->Draw("AP");
      gHLratio[i][j]->SetMarkerColor(kBlue);
      gHLratio[i][j]->SetMarkerStyle(8);
      gHLratio[i][j]->SetMarkerSize(0.5);
      gHLratio[i][j]->GetXaxis()->SetRangeUser(0, 3000);
      gHLratio[i][j]->GetYaxis()->SetRangeUser(0, 15000);
      gPad->Update();
      double pars[3] = {10, 0, 1500};
      fHGratio->SetParameters(pars);
      gHLratio[i][j]->Fit(fHGratio, "Q");
      ratio[5 * i + j] = fHGratio->GetParameter(0);
      tex->DrawLatexNDC(0.7, 0.3, Form("Slope=%.4lf", ratio[5 * i + j]));
      TPaveStats *ptstats = (TPaveStats *)gHLratio[i][j]->GetListOfFunctions()->FindObject("stats");
      if (ptstats)
      {
        ptstats->SetX1NDC(0.6);
        ptstats->SetX2NDC(0.95);
        ptstats->SetY1NDC(0.6);
        ptstats->SetY2NDC(0.9);
      }
      gPad->Modified();
      gPad->Update();
    }
  }

  TFile *fout = new TFile("./5x5Noise-" + filename + ".root", "RECREATE");
  fout->cd();
  cTemp1->Write();
  cTemp2->Write();
  cLNoise->Write();
  cHNoise->Write();
  cHLratio->Write();
  for (Int_t i = 0; i < 5; i++)
  {
    for (Int_t j = 0; j < 5; j++)
    {
      hTemp1[i][j]->Write();
      hTemp2[i][j]->Write();
      hLNoise[i][j]->Write();
      hHNoise[i][j]->Write();
      gHLratio[i][j]->Write();
    }
  }
  fout->Close();
  std::ofstream outfile("parameters.txt");
  outfile << "temperature1:\n";
  for (int i = 0; i < 25; i++)
    outfile << temperature1[i] << ", ";
  outfile << "\ntemperature2:\n";
  for (int i = 0; i < 25; i++)
    outfile << temperature2[i] << ", ";
  outfile << "\nHGpedestal:\n";
  for (int i = 0; i < 25; i++)
    outfile << HGpedestal[i] << ", ";
  outfile << "\nLGpedestal:\n";
  for (int i = 0; i < 25; i++)
    outfile << LGpedestal[i] << ", ";
  outfile << "\nHGnoise:\n";
  for (int i = 0; i < 25; i++)
    outfile << HGnoise[i] << ", ";
  outfile << "\nLGnoise:\n";
  for (int i = 0; i < 25; i++)
    outfile << LGnoise[i] << ", ";
  outfile << "\ngain ratio:\n";
  for (int i = 0; i < 25; i++)
    outfile << ratio[i] << ", ";
  outfile.close();
  std::cout << "generate root file: " << "./5x5Noise-" + filename + ".root" << ", and parameter file: parameter.txt" << std::endl;
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv, char *argc[])
{
  if (argv < 2)
  {
    cout << "Usage: ./Draw5x5NoiseTempGratio decode file" << endl;
    return 1;
  }
  TString rootname = argc[1];
  Draw5x5NoiseTempGratio(rootname);
  return 0;
}
#endif
