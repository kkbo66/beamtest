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
#include <TMath.h>
#include <TString.h>

using namespace std;
using namespace TMath;

void getwave(TString rootname, int index1, int index2){


  TFile *f1 = new TFile(rootname);
  TTree *t1 = (TTree*)f1->Get("decode_data");
  Int_t nentries = (Int_t)t1->GetEntries();

  vector<Double_t> vLowGainWaveform;
  vector<Double_t> vHighGainWaveform;
  vLowGainWaveform.clear();
  vHighGainWaveform.clear();

  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      if((i!=index1)||(j!=index2)) continue;
      TTreeReader myReader(t1);
      TTreeReaderValue<Double_t> LowGainPedestal(myReader, Form("Hit_%d_%d.LowGainPedestal", i+1, j+1));
      TTreeReaderValue<Double_t> HighGainPedestal(myReader, Form("Hit_%d_%d.HighGainPedestal", i+1, j+1));
      TTreeReaderArray<Double_t> LAmplitude(myReader, Form("Hit_%d_%d.LAmplitude", i+1, j+1));
      TTreeReaderArray<Double_t> HAmplitude(myReader, Form("Hit_%d_%d.HAmplitude", i+1, j+1));
      while (myReader.Next()) {
        Double_t LPeak = -1;
        Double_t HPeak = -1;
        for(size_t k=0; k<LAmplitude.GetSize(); k++){
          double Amplitude = LAmplitude[k] - *LowGainPedestal;
          if(Amplitude > LPeak) LPeak = Amplitude;
        }
        for(size_t k=0; k<HAmplitude.GetSize(); k++){
          double Amplitude = HAmplitude[k] - *HighGainPedestal;
          if(Amplitude > HPeak) HPeak = Amplitude;
        }
        if(HPeak > 500 && HPeak < 14000){
          for(size_t k=0; k<HAmplitude.GetSize(); k++){
            double Amplitude = HAmplitude[k] - *HighGainPedestal;
            vHighGainWaveform.push_back(Amplitude);
          }
          for(size_t k=0; k<LAmplitude.GetSize(); k++){
            double Amplitude = LAmplitude[k] - *LowGainPedestal;
            vLowGainWaveform.push_back(Amplitude);
          }
        }
      }
    }
  }

  ofstream outfile;
  outfile.open(Form("Waveform_HighGain_Ch%d_%d.txt", index1, index2), ios::out);
  for(size_t i=0; i<vHighGainWaveform.size(); i++){
    int highGainValue = static_cast<int>(vHighGainWaveform[i]);
    outfile<<highGainValue<<endl;
  }
  outfile.close();
  outfile.open(Form("Waveform_LowGain_Ch%d_%d.txt", index1, index2), ios::out);
  for(size_t i=0; i<vLowGainWaveform.size(); i++){
    int lowGainValue = static_cast<int>(vLowGainWaveform[i]);
    outfile<<lowGainValue<<endl;
  } 
  outfile.close();


}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv<2)
   {
      cout<<"Usage: ./getPeak rootfile or ./getPeak (rootlist).txt"<<endl;
      return 1;
   }
   for(Int_t i=0; i<argv; i++){
      cout<<argc[i]<<endl;
   }
    if(string(argc[1]).find(".txt")!=string::npos){
      ifstream infile;
      infile.open(argc[1], ios::in);
      string line;
      while(getline(infile, line)){
        TString rootname=line;
        getPeak(rootname);
      }
      infile.close();
      return 0;
    }
    else if(string(argc[1]).find(".root")==string::npos){
      cout<<"Please input root file or (rootlist).txt"<<endl;
      return 1;
    }
    TString rootname=argc[1];
    getPeak(rootname);
    return 0;
}
#endif

