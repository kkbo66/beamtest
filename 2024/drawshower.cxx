#include"/home/bwang/headfile/bes3plotstyle.c"
using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

Vdouble DoFit(TH1F *h, TF1 *f1, TCanvas* c, int subc, TString xname, double perbin){
	c->cd(subc);
	gPad->SetGrid();
	bool swith=false;
  double mean = 0;
  double sigma = 0;
  Vdouble Vout;
  Vout.clear();
	for(int i=0;i<16;i++){
		double a=0.99+0.01*double(i+1);
		for(int j=0;j<16;j++){
			double b=0.030+0.0001*double(j);
			f1->SetParameters(500,h->GetMean()*a,b,0.6,2.8);
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
			if(sqrt(Error1)>0&&sqrt(Error1)<0.2&&(left1+right1)>0) swith=true;
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
				TString p11=Form("%.4f",f1->GetParameter(1)/1000.0);
				TString s11="Mean="+p11+"GeV";
				
				TString p21=Form("%.3f",(left1+right1)/(2*sqrt(log(4.0))));
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
			if(swith==true) break;	
		}
		if(swith==true) break;
	}
  Vout.push_back(mean);
  Vout.push_back(sigma);
  return Vout;
}

void drawshower(TString val){

	SetPrelimStyle();
	SetStyle();
	gStyle->SetOptStat(0);

  //double E5x5[100];
  vector<double> *E5x5 = 0;

	TString ecalpath;
  TChain *chain;
  chain = new TChain("rec_data");
  ecalpath = "../build/";
  chain->Add(ecalpath+val+".root");
  TBranch *branch = chain->GetBranch("ShowerE5x5");
  chain->SetBranchAddress("ShowerE5x5", &E5x5);

		TCut cut = "";

	double bin = 100;
	double lo = 500;
	double up = 1200;

	TString xname = "E_{5x5}";
	TH1F *hecal = new TH1F("hecal", "", bin, lo, up);

  for(int i=0; i<chain->GetEntries(); i++){
    chain->GetEntry(i);
    for(int j=0; j<E5x5->size(); j++){  
      hecal->Fill((*E5x5)[j]);
    }
  }

  TF1 *f1 = new TF1("f1","crystalball",lo*1.01,up*0.95);  
  f1->SetNpx(1000);

  double mean;
  double sigma;
  TCanvas *c = new TCanvas("c","Energy1000MeV",10,10,600,500);
  Vdouble Vout = DoFit(hecal,f1,c,1,xname,(up-lo)/bin);
  mean = Vout[0];
  sigma = Vout[1];

  cout << "Energy: " << 1000 << " MeV" << endl;
  cout << "Mean: " << mean << " GeV" << endl;
  cout << "Sigma/Mean: " << sigma << "%" << endl;
}

