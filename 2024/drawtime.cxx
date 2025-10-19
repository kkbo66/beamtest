#include"/home/bwang/headfile/bes3plotstyle.c"
using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

double DoFit(TH1F *h, TF1 *f1, TCanvas* c, int subc, TString xname, double perbin){
	c->cd(subc);
	gPad->SetGrid();
	bool swith=false;
  double sigma = 0;
	for(int i=0;i<3;i++){
			h->Fit(f1,"ESR");
			double Mean1=f1->GetParameter(1);
			double Sigma1=f1->GetParameter(2);
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
				TString s11="Mean="+p11+"ns";
				
				TString p21=Form("%.3f",f1->GetParameter(2)*1000);
				TString s21="Sigma="+p21+"ps";
				TString p31=Form("%.2f",f1->GetParameter(2)/Mean1*100);
				TString s31="Sigma/Mean="+p31+"%";
        sigma = f1->GetParameter(2)*1000; 
				
				TText *text1;
				text1= pt11->AddText(s11);
				text1= pt21->AddText(s21);
				//text1= pt31->AddText(s31);

				h->SetYTitle(Form("Events /%.2f(ps)",1000*perbin));
				h->SetXTitle(xname);
				h->GetYaxis()->SetLabelSize(0.06);
				h->GetYaxis()->SetTitleSize(0.06);
				h->GetXaxis()->SetLabelSize(0.06);
				h->GetXaxis()->SetTitleSize(0.06);
        h->GetXaxis()->SetTitleOffset(1);
        h->GetYaxis()->SetTitleOffset(1.5);

				int color;
				if(subc==1) color = kBlue;
				else color = kRed;
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
				if(subc==1) label = "ECAL";
				else label = "BES";
				leg2->AddEntry(h,label,"l");
				leg2->Draw();
	}
  return sigma;
}

void drawtime(TString val, int t0){

	SetPrelimStyle();
	SetStyle();
	gStyle->SetOptStat(0);

  vector<double> *HitTime = 0;
  vector<double> *HitEnergy = 0;

	TString ecalpath;
  TChain *chain;
  chain = new TChain("rec_data");
  ecalpath = "../build/";
  chain->Add(ecalpath+val+".root");
  
  chain->SetBranchAddress("HitTime", &HitTime);
  chain->SetBranchAddress("HitEnergy", &HitEnergy);

    TCut cut = "";

  double bin = 100;
  double lo = t0-20;
  double up = t0+20;

  TString xname = "Time/ns";
  TH1F *hecal = new TH1F("hecal", "", bin, lo, up);

  for(int i=0; i<chain->GetEntries(); i++){
    chain->GetEntry(i);
    double maxE = 0;
    int maxEindex = -1;
    //cout<<"------"<<endl;
    for(int j=0; j<HitEnergy->size(); j++){
      //cout<<HitEnergy->at(j)<<" "<<HitTime->at(j)<<endl;
      if(HitEnergy->at(j)>maxE){
        maxE = HitEnergy->at(j);
        maxEindex = j;
      }
    }
    //if(maxE>0) cout<<maxEindex<<" "<<maxE<<" "<<HitTime->at(maxEindex)<<endl;
    if(maxE>0) hecal->Fill(HitTime->at(maxEindex));
  }

  TF1 *f1 = new TF1("f1","gaus",lo+0.3,up-0.3);
  f1->SetNpx(1000);

  double sigma;
  TCanvas *c = new TCanvas("c","Time",10,10,600,500);
  sigma = DoFit(hecal,f1,c,1,xname,(up-lo)/bin);
}


