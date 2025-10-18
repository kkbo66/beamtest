#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMath.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TAxis.h>

using namespace std;
using namespace TMath;

void drawunwave(int index1, int index2, int num){
  
  gROOT->SetBatch(kTRUE);

  TFile *f = new TFile("/home/kkbo/beamtest/root/re.root");
  //TFile *f = new TFile("/home/kkbo/beamtest/2025/ECAL/rootfile/data_EBS_20251012T131808_0.root");
  TTree *t = (TTree*)f->Get("decode_data");
  int entries = t->GetEntries();

  TTreeReader myReader(t);
  TTreeReaderArray<Double_t> LAmp(myReader, Form("Hit_%d_%d.LAmplitude", index1, index2));
  TTreeReaderArray<Double_t> HAmp(myReader, Form("Hit_%d_%d.HAmplitude", index1, index2));

  const int cnum = entries < num ? entries : num;
  //const int cnum = 100;
  TCanvas *c[cnum];
  TCanvas *c2[cnum];
  int npoints = 256;
  int n = 0;
  while(myReader.Next()){
    if(n >= num) break;
    //const Double_t *la = LAmp.Get();
    //const Double_t *ha = HAmp.Get();
    bool iflarge = false;
    for(int i=128; i<npoints; i++){
      if(LAmp[i] > 4000 || HAmp[i] > 4000){
        iflarge = true;
        break;
      }
    }
    if(!iflarge) continue;

    c[n] = new TCanvas(Form("c_%d_%d_%d", index1, index2, n), Form("c_%d_%d_%d", index1, index2, n), 800, 600); 
    //TCanvas *c = new TCanvas("c", "c", 800, 600);
    c[n]->SetGrid();
    TGraph *gla = new TGraph();
    for(int i=0; i<npoints; i++){
      //gla->SetPoint(i, i, la[i]);
      gla->SetPoint(i, i*12.5, LAmp[i]);
    }
    gla->SetLineColor(kBlue);
    gla->SetLineWidth(2);
    gla->GetXaxis()->SetTitle("Time (ns)");
    gla->GetYaxis()->SetTitle("ADC");
    gla->Draw("AL");
    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->AddEntry(gla, "LAmp", "l");
    leg->Draw();
    //c[n]->Print(Form("/home/kkbo/beamtest/draw/pdf/lAmp_%d_%d_%d.pdf", index1, index2, n));

    c2[n] = new TCanvas(Form("c2_%d_%d_%d", index1, index2, n), Form("c2_%d_%d_%d", index1, index2, n), 800, 600);
    //TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
    c2[n]->SetGrid();
    TGraph *gha = new TGraph();
    for(int i=0; i<npoints; i++){
      gha->SetPoint(i, i*12.5, HAmp[i]);
    }
    gha->SetLineColor(kRed);
    gha->SetLineWidth(2);
    gha->GetXaxis()->SetTitle("Time (ns)");
    gha->GetYaxis()->SetTitle("ADC");
    gha->Draw("AL");
    TLegend *leg2 = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg2->SetFillColor(0);
    leg2->SetFillStyle(0);
    leg2->SetBorderSize(0);
    leg2->AddEntry(gha, "HAmp", "l");
    leg2->Draw();
    //c2[n]->Print(Form("/home/kkbo/beamtest/draw/pdf/hAmp_%d_%d_%d.pdf", index1, index2, n));

    n++;
  }

  TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/unwave.root", "UPDATE");
    for(int i=0; i<n; i++){
      c[i]->Write(Form("lAmp_%d_%d_%d", index1, index2, i));
      c2[i]->Write(Form("hAmp_%d_%d_%d", index1, index2, i));
    }
  fout->Close();

  
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv>4) 
   {
      cout<<"please input 3 parameters: index1 index2 num"<<endl;
      cout<<"or input 1 parameter: num"<<endl;
      cout<<"or index1 and index2 with all events"<<endl;
      cout<<"or no parameters with all events and all index1 and index2"<<endl;
      return -1;
   }
   TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/unwave.root", "RECREATE");
   delete fout;
   if(argv==2){
     for(int i=0; i<5; i++){
       for(int j=0; j<5; j++){
         drawunwave(i+1, j+1, atoi(argc[1]));
       }
     }
     return 0;
   }
   else if(argv==4){
     drawunwave(atoi(argc[1]), atoi(argc[2]), atoi(argc[3]));
     return 0;
   }
   else if(argv==3){
     drawunwave(atoi(argc[1]), atoi(argc[2]), 1000000);
     return 0;
   }
   else if(argv==1){
     for(int i=0; i<5; i++){
       for(int j=0; j<5; j++){
         drawunwave(i+1, j+1, 1000000);
       }
     }
     return 0;
   }
}
#endif
