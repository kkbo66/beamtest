#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TStyle.h>
#include <TChain.h>

using namespace std;
using namespace TMath;

double calculateMean(const vector<Double_t>& data) {
    Double_t sum = 0;
    for (const auto& value : data) {
        sum += value;
    }
    return sum / data.size();
}

double calculateStdDev(const vector<Double_t>& data, double mean) {
    Double_t sum = 0;
    for (const auto& value : data) {
        sum += (value - mean) * (value - mean);
    }
    return sqrt(sum / (data.size() - 1));
}


void getnosped(vector<TString> rootfiles){

  TChain *chain = new TChain("decode_data");
  for(auto& file : rootfiles){
    chain->Add(file);
    cout<<"Added file: "<<file<<endl;
  }
  int nEntries = chain->GetEntries();
  cout<<"Total entries in chain: "<<nEntries<<endl;

  ofstream outhfile("/home/kkbo/beamtest/draw/par/forre/highgain_noise_stats.txt");
  ofstream outlfile("/home/kkbo/beamtest/draw/par/forre/lowgain_noise_stats.txt");
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      cout<<"Processing Hit_"<<i+1<<"_"<<j+1<<endl;
      TTreeReader myReader(chain);
      TTreeReaderArray<Double_t> LNoise(myReader, Form("Hit_%d_%d.LNoise", i+1, j+1));
      TTreeReaderArray<Double_t> HNoise(myReader, Form("Hit_%d_%d.HNoise", i+1, j+1));
      vector<Double_t> vLNoise;
      vector<Double_t> vHNoise;
      vLNoise.clear();
      vHNoise.clear();
      while (myReader.Next()) {
        for(Int_t k=1; k<LNoise.GetSize(); k++){
          vLNoise.push_back(LNoise[k]);
          vHNoise.push_back(HNoise[k]); 
        }
      }
      double orgin_meanL = calculateMean(vLNoise);
      double orgin_stddevL = calculateStdDev(vLNoise, orgin_meanL);
      double orgin_meanH = calculateMean(vHNoise);
      double orgin_stddevH = calculateStdDev(vHNoise, orgin_meanH);
      vector<Double_t> vLNoise_new;
      vector<Double_t> vHNoise_new;
      vLNoise_new.clear();
      vHNoise_new.clear();
      for(auto& val : vLNoise){
        if(Abs(val - orgin_meanL) <= 5*orgin_stddevL){
          vLNoise_new.push_back(val);
        }
      }
      for(auto& val : vHNoise){
        if(Abs(val - orgin_meanH) <= 5*orgin_stddevH){
          vHNoise_new.push_back(val);
        }
      }
      double new_meanL = calculateMean(vLNoise_new);
      double new_stddevL = calculateStdDev(vLNoise_new, new_meanL);
      double new_meanH = calculateMean(vHNoise_new);
      double new_stddevH = calculateStdDev(vHNoise_new, new_meanH);
      outlfile<< new_meanL <<" "<< new_stddevL <<endl;
      outhfile<< new_meanH <<" "<< new_stddevH <<endl;
    }
  }
  outhfile.close();
  outlfile.close();
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
   if(argv<2)
   {
      cout<<"Please input root file or (rootlist).txt"<<endl;
      return 1;
   }
   for(Int_t i=0; i<argv; i++){
      cout<<argc[i]<<endl;
   }
   vector<TString> rootfiles;
   rootfiles.clear();
   if(string(argc[1]).find(".txt")!=string::npos){
     ifstream infile;
     infile.open(argc[1], ios::in);
     string line;
     while(getline(infile, line)){
       TString rootname=line;
       rootfiles.push_back(rootname);
     }
     infile.close();
   }
   else if(string(argc[1]).find(".root")==string::npos){
     cout<<"Please input root file or (rootlist).txt"<<endl;
     return 1;
   }
    else{
      TString rootname=argc[1];
      rootfiles.push_back(rootname);
    }
   TString rootname=argc[1];
   getnosped(rootfiles);
}
#endif


