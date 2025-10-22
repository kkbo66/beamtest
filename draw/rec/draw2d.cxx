#include <iostream>
using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

void draw2d(string rootfile, double energy){

	gStyle->SetOptStat(0);
  vector<string> rootlist;
  rootlist.clear();
  if(rootfile.find(".txt")!=string::npos){
    ifstream infile;
    infile.open(rootfile.c_str());
    string line;
    while(getline(infile,line)){
      rootlist.push_back(line);
    }
  }
  else if(rootfile.find(".root")!=string::npos){
    rootlist.push_back(rootfile);
  }
  else{
    cout<<"Please input root file"<<endl;
    return;
  }

  TChain *t = new TChain("rec_data");
  for(unsigned int i=0;i<rootlist.size();i++){
    t->Add(rootlist[i].c_str());
    cout<<"Add root file: "<<rootlist[i]<<endl;
  }

  double low_shower = 0.5*energy/1000;
  double high_shower = 1.2*energy/1000;
  double low_hit = 0.4*energy/1000;
  double high_hit = energy/1000;

  vector<int> *SeedID = 0;
  vector<int> *HitID = 0;
  vector<double> *Energy_5x5 = 0;
  vector<double> *Energy_Hit = 0;
  t->SetBranchAddress("ShowerID",&SeedID);
  t->SetBranchAddress("ShowerE5x5",&Energy_5x5);
  t->SetBranchAddress("HitID",&HitID);
  t->SetBranchAddress("HitEnergy",&Energy_Hit);

  TH2F *h2 = new TH2F("h2","h2",100,low_shower,high_shower,100,low_hit,high_hit);
  for(int i=0;i<t->GetEntries();i++){
    t->GetEntry(i);
    for(unsigned int j=0;j<SeedID->size();j++){
      if(SeedID->at(j)==326034){ // center crystal ID for ECAL
        double shower_energy = Energy_5x5->at(j)/1000;
        for(unsigned int k=0;k<HitID->size();k++){
          if(HitID->at(k)==326034){
            double hit_energy = Energy_Hit->at(k)/1000;
            h2->Fill(shower_energy,hit_energy);
            break;
          }
        }
      }
    }
  }
  TCanvas *c1 = new TCanvas("c1","c1",800,600);    
  h2->Draw("COLZ");

}
