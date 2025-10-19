#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>

using namespace std;
using namespace TMath;

void gethmax(){
  
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

  ofstream fout("/home/kkbo/beamtest/draw/par/forre/HighGainLimit.txt");
  for(int i=0; i<5; i++){
    for(int j=0; j<5; j++){
      double hmax = 0;
      int count = 0;
      for(Int_t entry=0; entry<tree->GetEntries(); entry++){
        tree->GetEntry(entry);
        if(i==1 && j==3){
          if(vLowGainMIP[i][j] > 3200){
            hmax += vHighGainMIP[i][j];
            count++;
            //cout << "Entry: " << entry << ", Channel: (" << i+1 << "," << j+1 << "), Low Gain MIP: " << vLowGainMIP[i][j] << ", High Gain MIP: " << vHighGainMIP[i][j] << endl;
          }
        }
        else if(vLowGainMIP[i][j] > 1600){
          hmax += vHighGainMIP[i][j];
          count++;
          //cout << "Entry: " << entry << ", Channel: (" << i+1 << "," << j+1 << "), Low Gain MIP: " << vLowGainMIP[i][j] << ", High Gain MIP: " << vHighGainMIP[i][j] << endl;
        }
      }
      if(count > 0){
        hmax /= count;
      }
      fout << hmax << endl;
    }
  }
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
  if(argv<1)
  {
      cout<<"Usage: ./gethmax"<<endl;
      return 1;
  }
  gethmax();
  return 0;
}
#endif  
