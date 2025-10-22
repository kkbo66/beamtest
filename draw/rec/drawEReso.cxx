using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

void drawEReso(){
  ifstream file("EnergyResolution.txt");
  Vdouble Energy, Resolution;
  Energy.clear();
  Resolution.clear();
  double e, r;
  while(file >> e >> r){
    Energy.push_back(e);
    Resolution.push_back(r);
  }
  int nPoints = Energy.size();
  TGraph *g = new TGraph(nPoints);
  for(int i=0; i<nPoints; i++){
    g->SetPoint(i, Energy[i]/1000, Resolution[i]);
  }
  TCanvas *c = new TCanvas("cEReso", "Energy Resolution", 800, 600);
  c->SetGrid();
  g->SetTitle("Energy Resolution vs Energy;Energy (GeV);Energy Resolution (%)");
  g->GetXaxis()->SetTitle("Energy (GeV)");
  g->GetYaxis()->SetTitle("Energy Resolution (%)");
  g->SetMarkerStyle(20);
  g->SetMarkerColor(kBlue+1);
  g->Draw("AP");

  TFile *f = new TFile("/home/kkbo/beamtest/draw/figureroot/ecal.root","update");
  if(f->IsOpen()){
    TDirectory *exist = (TDirectory*)f->Get("Energy");
    if(!exist) exist = f->mkdir("Energy"); 
    exist->cd();
    TCanvas *c_exist = (TCanvas*)exist->Get("cEReso");
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
