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
      TF1 *fitLow = new TF1("fitLow", "[0]*TMath::Voigt(x-[1], [2], [3])", maxvalue-250, maxvalue+500);
      fitLow->SetParameters(100, maxvalue*0.8, 50, 50);
      TFitResultPtr r = hLowGainMIP[i][j]->Fit(fitLow, "RS"); 
      for(int iter=0; iter<50; iter++){
        if(r->IsValid()&&r->Status()==0) break;
        else {
          fitLow->SetParameters(100, maxvalue*(0.8+0.02*(iter+1)), 50+10*(iter+1), 50+10*(iter+1));
          r = hLowGainMIP[i][j]->Fit(fitLow, "RS");
        }
      }
      fitLow->Draw("same");
      lowfile << Form("Low Gain MIP %d %d: ", i+1, j+1)
              << "Amplitude = " << fitLow->GetParameter(0) << ", "
              << "Mean = " << fitLow->GetParameter(1) << ", "
              << "Sigma = " << fitLow->GetParameter(2) << endl;
      lowpeakfile << fitLow->GetParameter(1) << endl;
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
      TF1 *fitHigh = new TF1("fitHigh", "[0]*TMath::Voigt(x-[1], [2], [3])", maxvalue-2500, maxvalue+5000);
      fitHigh->SetParameters(100, maxvalue*0.8, 500, 500);
      TFitResultPtr r = hHighGainMIP[i][j]->Fit(fitHigh, "RS");
      for(int iter=0; iter<50; iter++){
        if(r->IsValid()&&r->Status()==0) break;
        else {
          fitHigh->SetParameters(1, maxvalue*(0.8+0.02*(iter+1)), 500+100*(iter+1), 500+100*(iter+1));
          r = hHighGainMIP[i][j]->Fit(fitHigh, "RS");
        }
      }
      fitHigh->Draw("same");
      highfile << Form("High Gain MIP %d %d: ", i+1, j+1)
               << "Amplitude = " << fitHigh->GetParameter(0) << ", "
               << "Mean = " << fitHigh->GetParameter(1) << ", "
               << "Sigma = " << fitHigh->GetParameter(2) << endl;
      highpeakfile << fitHigh->GetParameter(1) << endl;
    }
  }
  
  TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/MIP_Fit_Results.root", "RECREATE");
  fout->cd();
  c1->Write();
  c2->Write();
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
