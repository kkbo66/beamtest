using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

void drawdiffpos(string rootfile, double energy){

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

  vector<double> posX;  
  vector<double> posY;
  posX.clear();
  posY.clear();
  TFile *ftracker = TFile::Open("/home/kkbo/beamtest/root/tracker/Tracker-step4-rec-26.root");
  TTree *ttracker = (TTree*)ftracker->Get("Track");
  int trkeventid;
  Double_t trkpos[3];
  ttracker->SetBranchAddress("event",&trkeventid);
  ttracker->SetBranchAddress("ecalextraHit",&trkpos);

  for(int i=0;i<ttracker->GetEntries();i++){
    ttracker->GetEntry(i);
    posX.push_back(trkpos[0]/10);
    posY.push_back(trkpos[1]/10);
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

  TString hname = Form("hposdiff_%dMeV",int(energy));
  TString hnameX = Form("hdiffposX_%dMeV",int(energy));
  TString hnameY = Form("hdiffposY_%dMeV",int(energy));
  TH2F *hpos = new TH2F(hname,"Difference between Tracker and Ecal Shower Position;X (cm);Y (cm)",100,-12.5,12.5,100,-12.5,12.5);
  TH1F *hposX = new TH1F(hnameX,"Difference in X between Tracker and Ecal Shower Position;X (cm)",100,-5,5);
  TH1F *hposY = new TH1F(hnameY,"Difference in Y between Tracker and Ecal Shower Position;Y (cm)",100,-5,5);
  for(int i=0;i<t->GetEntries();i++){
    t->GetEntry(i);
    //if(i>1000) break;
    for(unsigned int j=0;j<ShowerX->size();j++){
      if(Energy_5x5->at(j)<0.5*energy) continue;
      if(SeedID->at(j)!=326034) continue; // SeedID for 150 GeV electron beam
      int new_eventid = eventid;
      if(new_eventid<0 || new_eventid>=int(posX.size())) continue;
      double r_tracker = Sqrt(posX[new_eventid]*posX[new_eventid]+posY[new_eventid]*posY[new_eventid]);
      //if(abs(posX[new_eventid])>2.5 || abs(posY[new_eventid])>2.5) continue;
      //if(posX[new_eventid]>-1 || posX[new_eventid]<-2 || posY[new_eventid]>0.5 || posY[new_eventid]<-0.5) continue;
      //if(abs(ShowerX->at(j))>1 || abs(ShowerY->at(j))>1) continue;
      if(r_tracker>0){
        double dx = ShowerX->at(j) - posX[new_eventid];
        double dy = ShowerY->at(j) + posY[new_eventid];
        hpos->Fill(dx,dy);
        hposX->Fill(dx);
        hposY->Fill(dy);
        //hpos->Fill(posX[new_eventid],posY[new_eventid]);
        //hpos->Fill(ShowerX->at(j),ShowerY->at(j));
      }
    }
  }

  TCanvas *c = new TCanvas("c","c",800,600);
  hpos->GetXaxis()->SetTitle("X (cm)");
  hpos->GetYaxis()->SetTitle("Y (cm)");
  hpos->SetTitle(("Shower Position Distribution at "+to_string(int(energy))+" MeV").c_str());
  hpos->Draw("COLZ");

  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  hposX->GetXaxis()->SetTitle("X (cm)");
  hposX->SetTitle(("Shower Position X Difference at "+to_string(int(energy))+" MeV").c_str());
  hposX->Draw();

  TCanvas *c2 = new TCanvas("c2","c2",800,600);
  hposY->GetXaxis()->SetTitle("Y (cm)");
  hposY->SetTitle(("Shower Position Y Difference at "+to_string(int(energy))+" MeV").c_str());
  hposY->Draw();  

  TFile *f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","update");
  if(f->IsOpen()){
    TDirectory *exist = (TDirectory*)f->Get("Position");
    if(!exist) exist = f->mkdir("Position");
    exist->cd();
    hpos->Write();
    hposX->Write(); 
    hposY->Write();
    f->cd();
    f->Close();
  }
  else{
    f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","recreate");
    TDirectory *exist = (TDirectory*)f->Get("Position");
    if(!exist) exist = f->mkdir("Position");
    exist->cd();
    hpos->Write();
    hposX->Write();
    hposY->Write();
    f->cd();
    f->Close();
  }

}
