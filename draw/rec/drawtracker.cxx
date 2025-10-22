using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

void drawtracker(string rootfile, double energy){

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
  
  TChain *t = new TChain("Track");
  for(unsigned int i=0;i<rootlist.size();i++){
    t->Add(rootlist[i].c_str());
    cout<<"Add root file: "<<rootlist[i]<<endl;
  }

  int eventid;
  Double_t Pos[3];
  t->SetBranchAddress("event",&eventid);
  t->SetBranchAddress("ecalextraHit",&Pos);

  TString hname = Form("hpos_tracker_%dMeV",int(energy));
  TH2F *hpos = new TH2F(hname,"Tracker Position Distribution",100,-12.5,12.5,100,-12.5,12.5);
  int nentries = t->GetEntries();
  for(int i=0;i<nentries;i++){
    t->GetEntry(i);
    double r = Sqrt(Pos[0]*Pos[0]+Pos[1]*Pos[1]);
    //if(abs(Pos[0])>25||abs(Pos[1])>25) continue;
    if(r>0)
    hpos->Fill(Pos[0]/10,Pos[1]/10);
  }
  hpos->GetXaxis()->SetTitle("X [cm]");
  hpos->GetYaxis()->SetTitle("Y [cm]");
  hpos->SetTitle(Form("Tracker Position Distribution at %.0f GeV",energy));
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  hpos->Draw("COLZ");


  TFile *f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","update");
  if(f->IsOpen()){
    TDirectory *exist = (TDirectory*)f->Get("Position");
    if(!exist) exist = f->mkdir("Position");
    exist->cd();
    TH2F *old = (TH2F*)exist->Get(hname);
    if(old) exist->Delete(hname);
    hpos->Write();
    f->cd();
    f->Close();
  }
  else{
    f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","recreate");
    TDirectory *exist = (TDirectory*)f->Get("Position");
    if(!exist) exist = f->mkdir("Position");
    exist->cd();
    hpos->Write();
    f->cd();
    f->Close();
  }
}
