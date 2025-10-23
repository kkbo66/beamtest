using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

void drawpos(string rootfile, double energy){

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


  int eventid;
  vector<double> *ShowerX = 0;
  vector<double> *ShowerY = 0;
  vector<int> *SeedID = 0;
  vector<double> *Energy_5x5 = 0;
  t->SetBranchAddress("EventID",&eventid);
  t->SetBranchAddress("ShowerPosX5x5",&ShowerX);
  t->SetBranchAddress("ShowerPosY5x5",&ShowerY);
  t->SetBranchAddress("ShowerID",&SeedID);
  t->SetBranchAddress("ShowerE5x5",&Energy_5x5);
  
  TString hname = TString::Format("hpos_%.0fMeV",energy);
  TString hxname = TString::Format("hposx_%.0fMeV",energy);
  TString hyname = TString::Format("hposy_%.0fMeV",energy);
  TH2F *hpos = new TH2F(hname,"Shower Position Distribution",100,-12.5,12.5,100,-12.5,12.5);
  TH1F *hposx = new TH1F(hxname,"Shower X Position Distribution",100,-12.5,12.5);
  TH1F *hposy = new TH1F(hyname,"Shower Y Position Distribution",100,-12.5,12.5);
  for(int i=0;i<t->GetEntries();i++){
    t->GetEntry(i);
    for(unsigned int j=0;j<ShowerX->size();j++){
      if(SeedID->at(j)!=326034) continue;
      if(Energy_5x5->at(j)<energy*0.8 || Energy_5x5->at(j)>energy*1.2) continue;
      hpos->Fill(ShowerX->at(j),ShowerY->at(j));
      hposx->Fill(ShowerX->at(j));
      hposy->Fill(ShowerY->at(j));
    }
  }
  TCanvas *c = new TCanvas("c","c",800,600);
  hpos->GetXaxis()->SetTitle("X (cm)");
  hpos->GetYaxis()->SetTitle("Y (cm)");
  hpos->SetTitle(("Shower Position Distribution at "+to_string(int(energy))+" MeV").c_str());
  hpos->Draw("COLZ");
 
  TCanvas *cx = new TCanvas("cx","cx",800,600); 
  hposx->GetXaxis()->SetTitle("X (cm)");
  hposx->SetTitle(("Shower X Position Distribution at "+to_string(int(energy))+" MeV").c_str());
  hposx->Draw();

  TCanvas *cy = new TCanvas("cy","cy",800,600);
  hposy->GetXaxis()->SetTitle("Y (cm)");
  hposy->SetTitle(("Shower Y Position Distribution at "+to_string(int(energy))+" MeV").c_str());
  hposy->Draw();

  TFile *f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","update");
  if(f->IsOpen()){
    TDirectory *exist = (TDirectory*)f->Get("Position");
    if(!exist) exist = f->mkdir("Position");
    exist->cd();
    hpos->Write();
    hposx->Write();
    hposy->Write();
    f->cd();
    f->Close();
  }
  else{
    f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","recreate");
    TDirectory *exist = (TDirectory*)f->Get("Position");
    if(!exist) exist = f->mkdir("Position");
    exist->cd();
    hpos->Write();
    hposx->Write();
    hposy->Write();
    f->cd();
    f->Close();
  }

}
