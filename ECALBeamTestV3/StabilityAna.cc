#include <string>
#include <iostream>
#include <fstream>
#include "TString.h"
#include "data_model.hh"
#include <TFile.h>
#include <TProfile.h>
#include <TTree.h>
#include <TH1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TF1.h>
#include "TChain.h"

using namespace std;

bool isTextFile(const string& fileName){
  size_t dotPos = fileName.find_last_of('.');
  if(dotPos == std::string::npos) return false;

  std::string extension = fileName.substr(dotPos + 1);
  if(extension == "txt")
    return true;
  else 
    return false;
}

int main(int argc, char const *argv[]){
  vector<string> datafiles;

  if(isTextFile(argv[1])){
    ifstream filestream(argv[1]);
    if(filestream.is_open()){
      string filename;
      while(getline(filestream,filename)) datafiles.push_back(filename);
    }
    else cout<< "error in reading filelist  "<<endl;
  }
  else datafiles.push_back(argv[1]);

  TChain *tree = new TChain("decode_data");
  for(size_t i=0;i<datafiles.size();i++) tree->AddFile(datafiles.at(i).data());
  vector<decode_data_col*> Hit(25);
  string Name[25];
  for(int i=0;i<25;i++){
    int no;
    if(i%5==0) no=3; 
    if(i%5==1) no=4; 
    if(i%5==2) no=5; 
    if(i%5==3) no=1; 
    if(i%5==4) no=2; 
    Name[i]=to_string(i/5+1)+"_"+to_string(no);
  }
  for(int i=0;i<25;i++) Hit[i]= new decode_data_col();
  for(int i=0;i<25;i++){
    string name="Hit_"+Name[i];
    tree->SetBranchAddress(name.data(),Hit[i]);
  }

  TString outputfile;
  if(argc==3)
    outputfile = argv[2];
  else{  
    outputfile = "StabilityAna.root";
    cout << "Auto save file as StabilityAna.root..." << endl;
  }

  TFile *f = new TFile(outputfile.Data(),"recreate");

  
}
