#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMath.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TAxis.h>

#define _Npoints 256
#define _Nnoise 25
// set time window begin&end
#define _timewinb 40
#define _timewine 80

using namespace std;
using namespace TMath;

class DataModel2025
{
public:
    DataModel2025();
    ~DataModel2025();
    void Set(int, float, float, float[_Npoints], float[_Npoints], float[_Nnoise], float[_Nnoise], float, float, float, float);

    bool clear();

    // Hit infomation
    Long64_t CrystalID;
    Double_t Temperature1;
    Double_t Temperature2;
    Double_t LAmplitude[_Npoints];
    Double_t HAmplitude[_Npoints];
    Double_t LNoise[_Nnoise];
    Double_t HNoise[_Nnoise];

    Double_t LowGainPedestal;
    Double_t HighGainPedestal;
    Double_t LowGainPeak;
    Double_t HighGainPeak;
};

DataModel2025::DataModel2025()
{
    clear();
}

DataModel2025::~DataModel2025() {}

bool DataModel2025::clear()
{
    memset(LAmplitude, 0, sizeof(LAmplitude));
    memset(HAmplitude, 0, sizeof(HAmplitude));
    memset(LNoise, 0, sizeof(LNoise));
    memset(HNoise, 0, sizeof(HNoise));

    CrystalID = 0;
    Temperature1 = 0;
    Temperature2 = 0;
    LowGainPedestal = 0;
    HighGainPedestal = 0;
    LowGainPeak = 0;
    HighGainPeak = 0;

    return true;
}

void DataModel2025::Set(int crystalID, float tmpera1, float tmpera2, float rowal[_Npoints], float rowah[_Npoints], float rownl[_Nnoise], float rownh[_Nnoise], float mlplat, float mhplat, float mlpeak, float mhpeak)
{
    for (int i = 0; i < _Npoints; i++)
    {
        LAmplitude[i] = rowal[i];
        HAmplitude[i] = rowah[i];
    }

    for (int i = 0; i < _Nnoise; i++)
    {
        LNoise[i] = rownl[i];
        HNoise[i] = rownh[i];
    }

    CrystalID = crystalID;
    Temperature1 = tmpera1;
    Temperature2 = tmpera2;

    LowGainPedestal = mlplat;
    HighGainPedestal = mhplat;
    LowGainPeak = mlpeak;
    HighGainPeak = mhpeak;
}

void draw5x5(int num = 100000){
  
  gROOT->SetBatch(kTRUE);

  TFile *f = new TFile("/home/kkbo/beamtest/root/re.root");
  TTree *t = (TTree*)f->Get("decode_data");

  const int n = 256;
  Float_t Time[n];
  t->SetBranchAddress("Time", Time);
  vector<DataModel2025*> hit(25);
  for(int i=0; i<5; i++){
    for(int j=0; j<5; j++){
      hit[i*5+j] = new DataModel2025();
      t->SetBranchAddress(Form("Hit_%d_%d", i+1, j+1), hit[i*5+j]);
    }
  }
  TCanvas *c[num];
  TCanvas *c2[num];
  for(Int_t i=700; i<t->GetEntries(); i++){
    if(i>=num) break;
    t->GetEntry(i);
    c[i] = new TCanvas(Form("c_%d", i), Form("c_%d", i), 800, 600); 
    c[i]->SetGrid();
    c[i]->Divide(5,5);
    TGraph *gl[25];
    for(int j=0; j<25; j++){
      gl[j] = new TGraph();
      for(int k=0; k<n; k++){
        gl[j]->SetPoint(k, Time[k], hit[j]->LAmplitude[k]);
      }
      gl[j]->SetLineColor(kBlue);
      gl[j]->SetLineWidth(2);
      gl[j]->GetXaxis()->SetTitle("Time (ns)");
      gl[j]->GetYaxis()->SetTitle("ADC");
      c[i]->cd(j+1);
      gl[j]->Draw("AL");
      TLegend *leg = new TLegend(0.7, 0.7,  0.9, 0.9);
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->SetBorderSize(0);
      leg->AddEntry(gl[j], Form("LAmp_%d_%d", j/5+1, j%5+1), "l");
      leg->Draw();
    }
    //c[i]->Print(Form("/home/kkbo/beamtest/draw/pdf/lAmp_5x5_%d.pdf", i));
    c2[i] = new TCanvas(Form("c2_%d", i), Form("c2_%d", i), 800, 600);
    c2[i]->SetGrid();
    c2[i]->Divide(5,5);
    TGraph *gh[25];
    for(int j=0; j<25; j++){
      gh[j] = new TGraph();
      for(int k=0; k<n; k++){
        gh[j]->SetPoint(k, Time[k], hit[j]->HAmplitude[k]); 
      }
      gh[j]->SetLineColor(kRed);  
      gh[j]->SetLineWidth(2);
      gh[j]->GetXaxis()->SetTitle("Time (ns)");
      gh[j]->GetYaxis()->SetTitle("ADC");
      c2[i]->cd(j+1);
      gh[j]->Draw("AL");
      TLegend *leg2 = new TLegend(0.7, 0.7, 0.9, 0.9);
      leg2->SetFillColor(0);
      leg2->SetFillStyle(0);
      leg2->SetBorderSize(0);
      leg2->AddEntry(gh[j], Form("HAmp_%d_%d", j/5+1, j%5+1), "l");
      leg2->Draw();
    }
    //c2[i]->Print(Form("/home/kkbo/beamtest/draw/pdf/hAmp_5x5_%d.pdf", i));
    TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/wave5x5.root", "UPDATE");
    c[i]->Write(Form("lAmp_5x5_%d", i));
    c2[i]->Write(Form("hAmp_5x5_%d", i));
    fout->Close();
  }
    

}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv!=2) 
   {
      cout<<"input 1 parameter: num"<<endl;
      return -1;
   }
   TFile *fout = new TFile("/home/kkbo/beamtest/draw/figureroot/wave5x5.root", "RECREATE");
    fout->Close();
   draw5x5(atoi(argc[1]));
    return 0;
}
#endif
