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

void getPeak(TString rootname){


  //TFile *f1 = new TFile("/home/kkbo/beamtest/2025/ECAL/rootfile/"+rootname+".root");
  //TFile *f1 = new TFile("/home/kkbo/beamtest/2025/ECAL/build/"+rootname+".root");
  TFile *f1 = new TFile(rootname);
  TTree *t1 = (TTree*)f1->Get("decode_data");
  Int_t nentries = (Int_t)t1->GetEntries();

  vector<vector<Double_t>> vLowGainPedestal;
  vector<vector<Double_t>> vHighGainPedestal;
  vector<vector<Double_t>> vLowGainPeak;
  vector<vector<Double_t>> vHighGainPeak;
  vLowGainPedestal.clear();
  vHighGainPedestal.clear();
  vLowGainPeak.clear();
  vHighGainPeak.clear();

  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      TTreeReader myReader(t1);
      TTreeReaderValue<Double_t> LowGainPedestal(myReader, Form("Hit_%d_%d.LowGainPedestal", i+1, j+1));
      TTreeReaderValue<Double_t> HighGainPedestal(myReader, Form("Hit_%d_%d.HighGainPedestal", i+1, j+1));
      TTreeReaderArray<Double_t> LAmplitude(myReader, Form("Hit_%d_%d.LAmplitude", i+1, j+1));
      TTreeReaderArray<Double_t> HAmplitude(myReader, Form("Hit_%d_%d.HAmplitude", i+1, j+1));
      vector<Double_t> vLPed;
      vector<Double_t> vHPed;
      vector<Double_t> vLPeak;
      vector<Double_t> vHPeak;   
      vLPed.clear();
      vHPed.clear();
      vLPeak.clear();
      vHPeak.clear();
      while (myReader.Next()) {
        vLPed.push_back(*LowGainPedestal);
        vHPed.push_back(*HighGainPedestal);
        Double_t LPeak = -1;
        Double_t HPeak = -1;
        for(size_t k=0; k<LAmplitude.GetSize(); k++){
          if(LAmplitude[k] > LPeak) LPeak = LAmplitude[k];  
        }
        for(size_t k=0; k<HAmplitude.GetSize(); k++){
          if(HAmplitude[k] > HPeak) HPeak = HAmplitude[k];  
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

  size_t dotspos = rootname.Last('.');
  if(dotspos==string::npos) dotspos = rootname.Length();
  size_t slashpos = rootname.Last('/');
  if(slashpos==string::npos) slashpos = -1;
  TString shortname = rootname(slashpos+1, dotspos-slashpos-1);
  ofstream outfile0;
  ofstream outfile;
  outfile0.open("/home/kkbo/beamtest/draw/elecal/para/peakvalues_"+shortname+".txt", ios::out);
  outfile.open("/home/kkbo/beamtest/draw/elecal/para/all_"+shortname+".txt", ios::out);
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      outfile << Form("Hit_%d_%d.LowGainPedestal", i+1, j+1) << endl;
      for(size_t k=0; k<vLowGainPedestal[i*5+j].size(); k++){
        outfile << vLowGainPedestal[i*5+j][k] << " " ;
      }
      outfile << endl;
      outfile << Form("Hit_%d_%d.HighGainPedestal", i+1, j+1) << endl;
      for(size_t k=0; k<vHighGainPedestal[i*5+j].size(); k++){
        outfile << vHighGainPedestal[i*5+j][k] << " ";
      }
      outfile << endl;
      outfile << Form("Hit_%d_%d.LowGainPeak", i+1, j+1) << endl;
      for(size_t k=0; k<vLowGainPeak[i*5+j].size(); k++){
        outfile << vLowGainPeak[i*5+j][k] << " ";
      }
      outfile << endl;
      outfile << Form("Hit_%d_%d.HighGainPeak", i+1, j+1) << endl;
      for(size_t k=0; k<vHighGainPeak[i*5+j].size(); k++){
        outfile << vHighGainPeak[i*5+j][k] << " ";
      }
      outfile << endl;
      outfile << Form("Hit_%d_%d.LowGainPeak", i+1, j+1) << endl;
      for(size_t k=0; k<vLowGainPeak[i*5+j].size(); k++){
        outfile << vLowGainPeak[i*5+j][k] - vLowGainPedestal[i*5+j][k] << " ";
      }
      outfile << endl;
      outfile << Form("Hit_%d_%d.HighGainPeak", i+1, j+1) << endl;
      for(size_t k=0; k<vHighGainPeak[i*5+j].size(); k++){
        outfile << vHighGainPeak[i*5+j][k] - vHighGainPedestal[i*5+j][k] << " ";
      }
      outfile << endl;
      double LowGainMean = 0;
      double HighGainMean = 0;
      int nValidLow = 0;
      int nValidHigh = 0;
      for(size_t k=0; k<vLowGainPeak[i*5+j].size(); k++){
        //if(vLowGainPeak[i*5+j][k] - vLowGainPedestal[i*5+j][k] < 100.0) continue;
        LowGainMean += vLowGainPeak[i*5+j][k] - vLowGainPedestal[i*5+j][k];
        nValidLow++;
      }
      for(size_t k=0; k<vHighGainPeak[i*5+j].size(); k++){
        //if(vHighGainPeak[i*5+j][k] - vHighGainPedestal[i*5+j][k] < 100.0) continue;
        HighGainMean += vHighGainPeak[i*5+j][k] - vHighGainPedestal[i*5+j][k];
        nValidHigh++;
      }
      LowGainMean /= nValidLow;
      HighGainMean /= nValidHigh;
      outfile << "Number of valid Low Gain entries: " << nValidLow << endl;
      outfile << "Number of valid High Gain entries: " << nValidHigh << endl;
      outfile << "LowGainMean: " << LowGainMean << endl;
      outfile << "HighGainMean: " << HighGainMean << endl;
      //outfile0 <<  LowGainMean << endl;
      //outfile0 <<  HighGainMean << endl;
      outfile0 << Form("Hit_%d_%d.LowGainMean: " , i+1, j+1) << LowGainMean << endl;
      outfile0 << Form("Hit_%d_%d.HighGainMean: " , i+1, j+1) << HighGainMean << endl;
      outfile << endl;
    }
  }
  outfile0.close(); 
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

