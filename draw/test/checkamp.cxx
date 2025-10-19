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
#include <TMath.h>
#include <TString.h>

#define _Npoints 256
#define _Nnoise 25

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

void checkamp(TString rootname){

  size_t dospos = rootname.Last('.');
  size_t slashpos = rootname.Last('/');
  TString filename;
  if(dospos != string::npos && slashpos != string::npos){
    filename = rootname(slashpos+1, dospos-slashpos-1);
  }
  else if(dospos != string::npos && slashpos == string::npos){
    filename = rootname(0, dospos);
  }
  else if(dospos == string::npos && slashpos != string::npos){
    filename = rootname(slashpos+1, rootname.Length()-slashpos-1);
  }
  else{
    filename = rootname;
  }
  cout<<"Processing file: "<<rootname<<"("<<filename<<")"<<endl;

  TFile *f1 = new TFile(rootname);
  TTree *t1 = (TTree*)f1->Get("decode_data");
  Int_t nentries = (Int_t)t1->GetEntries();

  //const int n = 256;
  //Double_t Time[n];
  //t->SetBranchAddress("Time", Time);
  vector<DataModel2025*> hit(25);
  for(int i=0; i<5; i++){
    for(int j=0; j<5; j++){
      hit[i*5+j] = new DataModel2025();
      t1->SetBranchAddress(Form("Hit_%d_%d", i+1, j+1), hit[i*5+j]);
    }
  }

  vector<Double_t> vLAmplitude; 
  vector<Double_t> vHAmplitude; 
  vLAmplitude.clear();
  vHAmplitude.clear();
  vector<int> vLFirstIndex; 
  vector<int> vHFirstIndex;
  vLFirstIndex.clear();
  vHFirstIndex.clear();
  double cut = 7540*8;

  for(Int_t i=0; i<nentries; i++){
    t1->GetEntry(i);
    vector<double> lavg;
    vector<double> havg;
    lavg.clear();
    havg.clear();
    for(int j=0; j<256; j++){
      double lsum = 0;
      double hsum = 0;
      for(int k=0; k<25; k++){
        lsum += hit[k]->LAmplitude[j];
        hsum += hit[k]->HAmplitude[j];
      }
      //lsum /= 20;
      //hsum /= 20;
      lavg.push_back(lsum);
      havg.push_back(hsum);
    }
    double lmax = -1; 
    double hmax = -1;
    int lmaxi = -1;
    int hmaxi = -1;
    for(int j=0; j<256; j++){
      if(lavg[j]>lmax){
        lmax = lavg[j];
        lmaxi = j;
      }
      if(havg[j]>hmax){
        hmax = havg[j];
        hmaxi = j;
      }
    }
    int lstart, hstart;
    int firstindexl = -1;
    int firstindexh = -1;
    if(lmaxi-16<0) lstart = 0;
    else lstart = lmaxi-16;
    if(hmaxi-16<0) hstart = 0;
    else hstart = hmaxi-16;
    for(int j=lstart; j<lmaxi; j++){  
      if(lavg[j]>cut) {
        firstindexl = j;
        break;
      }
    }
    for(int j=hstart; j<hmaxi; j++){  
      if(havg[j]>cut) {
        firstindexh = j;
        break;
      }
    }
    //int lstart, hstart;
    //if(lmaxi-32<0) lstart = 0;
    //else lstart = lmaxi-32;
    //if(hmaxi-32<0) hstart = 0;
    //else hstart = hmaxi-32;
    //double lped = 0;
    //double hped = 0;
    //for(int j=lstart; j<lstart+16; j++) lped += lavg[j];
    //for(int j=hstart; j<hstart+16; j++) hped += havg[j];
    //lped /= 16;
    //hped /= 16;
    //vLAmplitude.push_back(lmax - lped);
    //vHAmplitude.push_back(hmax - hped);
    vLAmplitude.push_back(lmax);
    vHAmplitude.push_back(hmax);
    vLFirstIndex.push_back(firstindexl);
    vHFirstIndex.push_back(firstindexh);
  } 

  double lmax = -1;  
  double hmax = -1;  
  for(size_t i=0; i<vLAmplitude.size(); i++){
    if(vLAmplitude[i]>lmax) lmax = vLAmplitude[i];
    if(vHAmplitude[i]>hmax) hmax = vHAmplitude[i];
  }

  double lcount = 0;
  double hcount = 0;

  TH1F *hLAmplitude = new TH1F("hLAmplitude", "L Amplitude;Amplitude;Counts", 100, 0, lmax*1.2);
  TH1F *hHAmplitude = new TH1F("hHAmplitude", "H Amplitude;Amplitude;Counts", 100, 0, hmax*1.2);
  for(size_t i=0; i<vLAmplitude.size(); i++){
    hLAmplitude->Fill(vLAmplitude[i]);
    hHAmplitude->Fill(vHAmplitude[i]);  
    if(vLAmplitude[i]>cut) lcount++;
    if(vHAmplitude[i]>cut) hcount++;
    cout<<"Event "<<i<<": L Amplitude = "<<vLAmplitude[i]<<", First Index = "<<vLFirstIndex[i]<<"; H Amplitude = "<<vHAmplitude[i]<<", First Index = "<<vHFirstIndex[i]<<endl;
  }

  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  c1->SetGrid(); 
  hLAmplitude->SetLineColor(kBlue); 
  hLAmplitude->SetLineWidth(2); 
  hLAmplitude->Draw();
  c1->Print(Form("%s_LAmplitude.pdf", filename.Data()));
  TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
  c2->SetGrid();
  hHAmplitude->SetLineColor(kRed); 
  hHAmplitude->SetLineWidth(2); 
  hHAmplitude->Draw();
  c2->Print(Form("%s_HAmplitude.pdf", filename.Data()));

  cout<<"L Amplitude > "<<cut<<": "<<lcount/vLAmplitude.size()*100<<"% ("<<lcount<<"/"<<vLAmplitude.size()<<")"<<endl;
  cout<<"H Amplitude > "<<cut<<": "<<hcount/vHAmplitude.size()*100<<"% ("<<hcount<<"/"<<vHAmplitude.size()<<")"<<endl;

}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv<2)
   {
      cout<<"Usage: ./checkamp [rootfile]"<<endl;
      return 1;
   }
    TString rootname=argc[1];
    checkamp(rootname);
    return 0;
}
#endif

