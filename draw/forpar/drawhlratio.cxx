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

void drawhlratio(){

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

  TFile *f1 = new TFile("/home/kkbo/beamtest/draw/figureroot/MIP_Spectra.root", "READ");
  TTree *t1 = (TTree*)f1->Get("MIP_Spectra");
  Int_t nentries = (Int_t)t1->GetEntries();

  double vLowGainMIP[5][5];
  double vHighGainMIP[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      t1->SetBranchAddress(Form("LowGainMIP_%d_%d", i+1, j+1), &vLowGainMIP[i][j]);
      t1->SetBranchAddress(Form("HighGainMIP_%d_%d", i+1, j+1), &vHighGainMIP[i][j]);
    }
  }

  TGraph *ghl[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      ghl[i][j] = new TGraph();
    }
  }
  for(Int_t entry=0; entry<nentries; entry++){
    t1->GetEntry(entry);
    for(Int_t i=0; i<5; i++){
      for(Int_t j=0; j<5; j++){
        if(vLowGainMIP[i][j]>0 && vHighGainMIP[i][j]>0)
        ghl[i][j]->SetPoint(entry, vLowGainMIP[i][j], vHighGainMIP[i][j]);
      }
    }
  }

  ofstream outfile("/home/kkbo/beamtest/draw/par/forre/hlratio.txt");
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  c1->Divide(5,5);
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      //c1->cd(i*5+j+1);
      c1->cd(21+i-5*j);
      ghl[i][j]->SetTitle(Form("Channel (%d,%d); High Gain Amplitude; Low Gain Amplitude", i+1, j+1));
      ghl[i][j]->SetMarkerStyle(20);
      ghl[i][j]->SetMarkerSize(0.5);
      ghl[i][j]->Draw("AP");
      TF1 *f1 = new TF1("f1", "[0]*x", 0, 1300);
      f1->SetParameter(0, 10);
      ghl[i][j]->Fit("f1", "R");
      f1->SetLineColor(kRed);
      f1->Draw("same");
      double slope = f1->GetParameter(0);
      double slope_err = f1->GetParError(0);
      outfile << Form("Channel (%d,%d): Slope = %.4f ± %.4f", i+1, j+1, slope, slope_err) << endl;
    }
  }
  outfile.close();


  TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/hlratio.root", "RECREATE");
  fout->cd();
  c1->Write();
  fout->Close();

}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv<1)
   {
      cout<<"Usage: ./drawhlratio"<<endl; 
      return 1;
   }
    drawhlratio();
    return 0;
}
#endif

