using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

Vdouble DoFit(TH1F *h, TF1 *f1, TCanvas* c, TString xname, double perbin){
	c->cd();
	gPad->SetGrid();
	bool swith=false;
  double mean = 0;
  double sigma = 0;
  Vdouble Vout;
  Vout.clear();
	for(int i=0;i<16;i++){
		double a=0.98+0.01*double(i+1);
		for(int j=0;j<16;j++){
			double b=0.030+0.0001*double(j);
			f1->SetParameters(100,h->GetMean()*a,b,0.6,2.8);
      //f1->FixParameter(2,1.4e-02);
			h->Fit(f1,"ESR");
			double Mean1=f1->GetParameter(1);
			double Sigma1=f1->GetParameter(2);
			double A1=abs(f1->GetParameter(3));
			double N1=f1->GetParameter(4);
			double right1=Sigma1*sqrt(log(4.0));
			double left1 =Sigma1*((pow(2*exp(-0.5*A1*A1),1/N1)-1)*N1/A1+A1);
			double a1=(sqrt(log(4.0))+A1+(pow(2*exp(-0.5*A1*A1),1/N1)-1)*N1/A1)/Mean1/(2*sqrt(log(4.0)))*100;
			double b1=-(Sigma1*sqrt(log(4.0))+Sigma1*((pow(2*exp(-0.5*A1*A1),1/N1)-1)*N1/A1+A1))/Mean1/Mean1/(2*sqrt(log(4.0)))*100;
			double c1=Sigma1*(1-pow(2*exp(-0.5*A1*A1),1/N1)-(pow(2*exp(-0.5*A1*A1),1/N1)-1)*N1/A1/A1)/Mean1/(2*sqrt(log(4.0)))*100;
			double d1=Sigma1*((pow(2*exp(-0.5*A1*A1),1/N1)-1)/A1-pow(2*exp(-0.5*A1*A1),1/N1)*(log(2.0)+log(exp(-0.5*A1*A1)))/N1/A1)/Mean1/(2*sqrt(log(4.0)))*100;
			double Error1=a1*a1*f1->GetParError(2)*f1->GetParError(2)+b1*b1*f1->GetParError(1)*f1->GetParError(1)+c1*c1*f1->GetParError(3)*f1->GetParError(3)+d1*d1*f1->GetParError(4)*f1->GetParError(4);
			if(sqrt(Error1)>0&&sqrt(Error1)<5&&(left1+right1)>0) swith=true;
			if(swith==true){
				TPaveText *pt11= new TPaveText(0.4,0.8,0.4,0.8,"BRNDC");
				TPaveText *pt21= new TPaveText(0.4,0.7,0.4,0.7,"BRNDC");
				TPaveText *pt31= new TPaveText(0.4,0.6,0.4,0.6,"BRNDC");
				TPaveText *pt41= new TPaveText(0.4,0.5,0.4,0.5,"BRNDC");
				pt11->SetBorderSize(1);
				pt11->SetFillColor(6);
				pt11->SetTextAlign(20);
				pt11->SetTextSize(0.06);
				pt21->SetBorderSize(1);
				pt21->SetFillColor(6);
				pt21->SetTextAlign(20);
				pt21->SetTextSize(0.06);
				pt31->SetBorderSize(1);
				pt31->SetFillColor(6);                                                      
				pt31->SetTextAlign(20);
				pt31->SetTextSize(0.06);
				pt41->SetBorderSize(1);
				pt41->SetFillColor(6);
				pt41->SetTextAlign(20);
				pt41->SetTextSize(0.06);
				TString p11=Form("%.4f",f1->GetParameter(1));
				TString s11="Mean="+p11+"GeV";
				
				TString p21=Form("%.3f",(left1+right1)/(2*sqrt(log(4.0)))*1000);
				TString s21="Sigma="+p21+"MeV";
				TString p31=Form("%.2f",((left1+right1)/(2*sqrt(log(4.0))))/Mean1*100);
				TString p311=Form("%.2f",sqrt(Error1));
				TString s31="Sigma/Mean="+p31+"%";
        mean = f1->GetParameter(1);
        sigma = ((left1+right1)/(2*sqrt(log(4.0))))/Mean1*100;
				
				TText *text1;
				text1= pt11->AddText(s11);
				text1= pt21->AddText(s21);
				text1= pt31->AddText(s31);

				h->SetYTitle(Form("Events /%.2f(MeV/c^{2})",1000*perbin));
				h->SetXTitle(xname);
				h->GetYaxis()->SetLabelSize(0.06);
				h->GetYaxis()->SetTitleSize(0.06);
				h->GetXaxis()->SetLabelSize(0.06);
				h->GetXaxis()->SetTitleSize(0.06);
        h->GetXaxis()->SetTitleOffset(1);
        h->GetYaxis()->SetTitleOffset(1.5);

				int color;
				color = kBlue;
				h->SetLineColor(color);
				h->SetLineWidth(2);

				h->Draw("hist");
				f1->SetLineColor(kOrange);
				f1->Draw("same");

        pt11->Draw();
        pt21->Draw();
        pt31->Draw();
				
				TLegend *leg2 = new TLegend(0.3, 0.3, 0.5, 0.5);
				leg2->SetFillColor(0);
				leg2->SetFillStyle(0);
				leg2->SetBorderSize(0);
				TString label;
				label = "ECAL";
				leg2->AddEntry(h,label,"l");
				leg2->Draw();
			}	
			if(swith==true) break;	
		}
		if(swith==true) break;
	}
  Vout.push_back(mean);
  Vout.push_back(sigma);
  return Vout;
}

void drawtrackfitshower(string rootfile, double energy){

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
  TFile *ftracker = TFile::Open("/home/kkbo/beamtest/root/tracker/Tracker-step4-rec.root");
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

  string energy_str = to_string(int(energy))+"MeV";

  TChain *t = new TChain("rec_data");
  for(unsigned int i=0;i<rootlist.size();i++){
    t->Add(rootlist[i].c_str());
    cout<<"Add root file: "<<rootlist[i]<<endl;
  }

  int eventid;
  vector<int> *SeedID = 0;
  vector<int> *HitID = 0;
  vector<double> *Energy_5x5 = 0;
  vector<double> *Energy_Hit = 0;
  vector<double> *ShowerX = 0;
  vector<double> *ShowerY = 0;
  t->SetBranchAddress("EventID",&eventid);
  t->SetBranchAddress("ShowerID",&SeedID);
  t->SetBranchAddress("ShowerE5x5",&Energy_5x5);
  t->SetBranchAddress("HitID",&HitID);
  t->SetBranchAddress("HitEnergy",&Energy_Hit);
  t->SetBranchAddress("ShowerPosX5x5",&ShowerX);
  t->SetBranchAddress("ShowerPosY5x5",&ShowerY);

  double low = 0.5*energy/1000;
  //double high = 1.05*energy/1000;
  double high = 1.2*energy/1000;
  double seedcut = 0.2*energy/1000;
  TH1F *henergy_ecal = new TH1F("henergy_ecal","ECAL Energy Distribution",100,low,high);
  double perbin = (high-low)/100.0;
  for(int i=0;i<t->GetEntries();i++){
    t->GetEntry(i);
    for(unsigned int j=0;j<SeedID->size();j++){
      if(SeedID->at(j)==326034){ // center crystal ID for ECAL
        double seed_energy = -1;
        for(unsigned int k=0;k<HitID->size();k++){
          if(HitID->at(k)==326034){
            seed_energy = Energy_Hit->at(k)/1000;
            break;
          }        
        }
        //cout<<"Event: "<<i<<", Seed Energy: "<<seed_energy*1000<<" MeV"<< ", E5x5: "<<Energy_5x5->at(j)*1000<<" MeV"<<endl;
        int new_eventid = eventid;
        if(new_eventid<0 || new_eventid>=int(posX.size())) continue;
        double r_track = Sqrt(posX[new_eventid]*posX[new_eventid]+posY[new_eventid]*posY[new_eventid]);
        if(r_track<0.0000000001) continue;
        //if(abs(posX[new_eventid])>1 || abs(posY[new_eventid])>1) continue;
        //if(posX[new_eventid]>-1 || posX[new_eventid]<-2 || posY[new_eventid]>0.5 || posY[new_eventid]<-0.5) continue;
        if(seed_energy<seedcut) continue;
        //if(!(ShowerX->at(j)>-3 && ShowerX->at(j)<0 && ShowerY->at(j)<1.5 && ShowerY->at(j)>-1.5)) continue;
        henergy_ecal->Fill(Energy_5x5->at(j)/1000);
      }
    }
  }

  TString canvas_name = "c_"+energy_str;
  TCanvas *c = new TCanvas(canvas_name,canvas_name,800,600);
  TF1 *f1 = new TF1("f1","crystalball",low*1.01,high*0.95);
  f1->SetNpx(1000);

  double mean;
  double sigma;
  Vdouble Vfit = DoFit(henergy_ecal,f1,c,"E_{5x5} (GeV)",perbin);
  mean = Vfit[0];
  sigma = Vfit[1];
  cout<<"Event entries: "<<henergy_ecal->Integral()<<endl;
  cout<<"Energy: "<<energy_str<<", Mean: "<<mean*1000<<" MeV, Resolution: "<<sigma<<" %"<<endl;

  TFile *f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","update");
  if(f->IsOpen()){
    TDirectory *exist = (TDirectory*)f->Get("Energy");
    if(!exist) exist = f->mkdir("Energy"); 
    exist->cd();
    TCanvas *c_exist = (TCanvas*)exist->Get(canvas_name);
    if(c_exist) c_exist->Delete();
    c->Write();
    f->cd();
    f->Close();
  }
  else{
    f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","recreate");
    TDirectory *exist = (TDirectory*)f->Get("Energy");
    if(!exist) exist = f->mkdir("Energy");
    exist->cd();
    c->Write();
    f->cd();
    f->Close();
  }
}

