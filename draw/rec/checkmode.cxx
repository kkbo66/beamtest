using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

void checkmode(){
  cout<<"Check mode works!"<<endl;

  TFile *f1 = TFile::Open("/home/kkbo/beamtest/2025/ECAL/build/Reconstruction1.root");
  TTree *t1 = (TTree*)f1->Get("rec_data");
  TFile *f2 = TFile::Open("/home/kkbo/beamtest/2025/ECAL/build/Reconstruction2.root");
  TTree *t2 = (TTree*)f2->Get("rec_data");

  vector<int> *SeedID_1 = 0;
  vector<int> *HitID_1 = 0;
  vector<double> *Energy_Hit_1 = 0;
  vector<double> *Energy_5x5_1 = 0;
  vector<double> *ShowerX_1 = 0;
  vector<double> *ShowerY_1 = 0;
  vector<int> *SeedID_2 = 0;
  vector<int> *HitID_2 = 0;
  vector<int> *HitTime_2 = 0;
  vector<double> *Energy_Hit_2 = 0;
  vector<double> *Energy_5x5_2 = 0;
  vector<double> *ShowerX_2 = 0;
  vector<double> *ShowerY_2 = 0;
  t1->SetBranchAddress("ShowerID",&SeedID_1);
  t1->SetBranchAddress("ShowerE5x5",&Energy_5x5_1);
  t1->SetBranchAddress("HitID",&HitID_1);
  t1->SetBranchAddress("HitEnergy",&Energy_Hit_1);
  t1->SetBranchAddress("ShowerPosX5x5",&ShowerX_1);
  t1->SetBranchAddress("ShowerPosY5x5",&ShowerY_1);
  t2->SetBranchAddress("ShowerID",&SeedID_2);
  t2->SetBranchAddress("ShowerE5x5",&Energy_5x5_2);
  t2->SetBranchAddress("HitID",&HitID_2);
  t2->SetBranchAddress("HitTime",&HitTime_2);
  t2->SetBranchAddress("HitEnergy",&Energy_Hit_2);
  t2->SetBranchAddress("ShowerPosX5x5",&ShowerX_2);
  t2->SetBranchAddress("ShowerPosY5x5",&ShowerY_2);

  TH1F *hdiff_E5x5 = new TH1F("hdiff_E5x5","E5x5 difference between modes;#DeltaE5x5 (MeV);Entries",100,0,200);
  //TH1F *hdiff_HitE = new TH1F("hdiff_HitE","Hit Energy difference between modes;#DeltaE_{hit} (GeV);Entries",100,-0.1,0.1);
  TH1F *hdiff_SeedE = new TH1F("hdiff_SeedE","Seed Energy difference between modes;#DeltaE_{seed} (MeV);Entries",100,0,40);
  TH1F *hseedTime_2 = new TH1F("hseedTime_2","Seed Time distribution mode 2;Seed Time (ns);Entries",100,450,500);

  Long64_t nentries = t1->GetEntries();
  cout<<"Number of entries: "<<nentries<<endl;
  for (Long64_t i=0; i<nentries; i++) {
    t1->GetEntry(i);
    t2->GetEntry(i);
    int nshowers1 = SeedID_1->size();
    int nshowers2 = SeedID_2->size();
    for(int j=0; j<nshowers1; j++){
      int seed1 = SeedID_1->at(j);
      if(Energy_5x5_1->at(j) < 300) continue; // only consider showers with E5x5 > 50 MeV
      // Find matching seed in mode 2
      for(int k=0; k<nshowers2; k++){
        int seed2 = SeedID_2->at(k);
        if(seed1 == seed2){
          double e5x5_1 = Energy_5x5_1->at(j);
          double e5x5_2 = Energy_5x5_2->at(k);
          hdiff_E5x5->Fill(e5x5_1 - e5x5_2);
          double eseed_1 = 0;
          double eseed_2 = 0;
          double seedTime_2 = 0;
          for(size_t m=0; m<HitID_1->size(); m++){
            if(HitID_1->at(m) == seed1){
              eseed_1 = Energy_Hit_1->at(m);
              break;
            }
          }
          for(size_t n=0; n<HitID_2->size(); n++){
            if(HitID_2->at(n) == seed2){
              eseed_2 = Energy_Hit_2->at(n);
              seedTime_2 = HitTime_2->at(n);
              break;
            }
          }
          hdiff_SeedE->Fill(eseed_1 - eseed_2);
          hseedTime_2->Fill(seedTime_2);
        }
      }
    }
  }
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  hdiff_E5x5->Draw();
  TCanvas *c2 = new TCanvas("c2","c2",800,600);
  hdiff_SeedE->Draw();
  TCanvas *c3 = new TCanvas("c3","c3",800,600);
  hseedTime_2->Draw();
}
