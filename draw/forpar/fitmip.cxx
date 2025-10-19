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

using namespace std;
using namespace TMath;

Double_t landaugauss(Double_t *x, Double_t *par) {
    Double_t xx = x[0];
    Double_t mpv = par[0];
    Double_t sigmaL = par[1];
    Double_t sigmaG = par[2];
    Double_t norm = par[3];

    Double_t result = 0;
    Double_t step = sigmaG / 5.0;
    for (Double_t i = -5*sigmaG; i <= 5*sigmaG; i += step) {
        Double_t landau = TMath::Landau(xx - i, mpv, sigmaL);
        Double_t gauss  = TMath::Gaus(i, 0, sigmaG);
        result += landau * gauss * step;
    }
    return norm * result;
}

void fitmip(){
  
  TFile *fin = new TFile("/home/kkbo/beamtest/draw/figureroot/MIP_Spectra.root", "READ");
  TTree *tree = (TTree*)fin->Get("MIP_Spectra");

  double vLowGainMIP[5][5];
  double vHighGainMIP[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      tree->SetBranchAddress(Form("LowGainMIP_%d_%d", i+1, j+1), &vLowGainMIP[i][j]);
      tree->SetBranchAddress(Form("HighGainMIP_%d_%d", i+1, j+1), &vHighGainMIP[i][j]);
    }
  }

  TH1F *hLowGainMIP[5][5];
  TH1F *hHighGainMIP[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      hLowGainMIP[i][j] = new TH1F(Form("hLowGainMIP_%d_%d", i+1, j+1), Form("Low Gain MIP Spectrum %d %d", i+1, j+1), 100, 0, 1300);
      hHighGainMIP[i][j] = new TH1F(Form("hHighGainMIP_%d_%d", i+1, j+1), Form("High Gain MIP Spectrum %d %d", i+1, j+1), 100, 0, 13000);
    }
  }
  for(Int_t entry=0; entry<tree->GetEntries(); entry++){
    tree->GetEntry(entry);
    for(Int_t i=0; i<5; i++){
      for(Int_t j=0; j<5; j++){
        hLowGainMIP[i][j]->Fill(vLowGainMIP[i][j]);
        hHighGainMIP[i][j]->Fill(vHighGainMIP[i][j]);
      }
    }
  }

  ofstream lowfile("/home/kkbo/beamtest/draw/par/forre/parameters_mipfit_lowgain.txt");
  ofstream lowpeakfile("/home/kkbo/beamtest/draw/par/forre/LGMipPeak.dat");
  TCanvas *c1 = new TCanvas("c1", "MIP Fit Low Gain", 800, 600);
  c1->Divide(5, 5);
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      c1->cd(21+i-5*j);
      hLowGainMIP[i][j]->Draw();
      int maxbin = hLowGainMIP[i][j]->GetMaximumBin();
      double maxvalue = hLowGainMIP[i][j]->GetBinCenter(maxbin);
      TF1 *fitLow = new TF1("fitLow", landaugauss, maxvalue-200, maxvalue+500, 4);
      fitLow->SetParameters(maxvalue, 10, 50, 1);
      TFitResultPtr r = hLowGainMIP[i][j]->Fit(fitLow, "RS"); 
      for(int iter=0; iter<50; iter++){
        if(r->IsValid()&&r->Status()==0) break;
        else {
          fitLow->SetParameters(maxvalue*(0.99+0.002*(iter+1)), 10+10*(iter+1), 50+10*(iter+1),1);
          r = hLowGainMIP[i][j]->Fit(fitLow, "RS");
        }
      }
      fitLow->Draw("same");
      lowfile << Form("Low Gain MIP %d %d: ", i+1, j+1)
                << "MPV = " << fitLow->GetParameter(0) << ", "
                << "LandauWidth = " << fitLow->GetParameter(1) << ", "
                << "GaussSigma = " << fitLow->GetParameter(2) << endl;
      lowpeakfile << fitLow->GetMaximumX() << endl;
    }
  }
  lowfile.close();
  lowpeakfile.close();
  ofstream highfile("/home/kkbo/beamtest/draw/par/forre/parameters_mipfit_highgain.txt");
  ofstream highpeakfile("/home/kkbo/beamtest/draw/par/forre/HGMipPeak.dat");
  TCanvas *c2 = new TCanvas("c2", "MIP Fit High Gain", 800, 600);
  c2->Divide(5, 5);
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      c2->cd(21+i-5*j);
      hHighGainMIP[i][j]->Draw();
      int maxbin = hHighGainMIP[i][j]->GetMaximumBin();
      double maxvalue = hHighGainMIP[i][j]->GetBinCenter(maxbin);
      TF1 *fitHigh = new TF1("fitHigh", landaugauss, maxvalue-2000, maxvalue+5000, 4);
      fitHigh->SetParameters(maxvalue, 100, 600, 1);
      TFitResultPtr r = hHighGainMIP[i][j]->Fit(fitHigh, "RS");
      for(int iter=0; iter<50; iter++){
        if(r->IsValid()&&r->Status()==0) break;
        else {
          fitHigh->SetParameters(maxvalue*(0.99+0.002*(iter+1)), 100+100*(iter+1), 500+100*(iter+1),1);
          r = hHighGainMIP[i][j]->Fit(fitHigh, "RS");
        }
      }
      fitHigh->Draw("same");
      highfile << Form("High Gain MIP %d %d: ", i+1, j+1)
                << "MPV = " << fitHigh->GetParameter(0) << ", "
                << "LandauWidth = " << fitHigh->GetParameter(1) << ", "
                << "GaussSigma = " << fitHigh->GetParameter(2) << endl;
      highpeakfile << fitHigh->GetMaximumX() << endl;
    }
  }
  
  TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/MIP_Fit_Results.root", "RECREATE");
  fout->cd();
  c1->Write();
  //c2->Write();
  fout->Close();
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
  if(argv<1)
  {
      cout<<"Usage: ./fitmip"<<endl;
      return 1;
  }
  fitmip();
  return 0;
}
#endif  
