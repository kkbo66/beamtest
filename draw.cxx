using namespace std;
using namespace TMath;

void draw(TString name) {
  cout << "Hello, World!" << endl;
  TFile *f1 = new TFile("/home/kkbo/beamtest/draw/figureroot/val-" + name);
  TCanvas *c1 = (TCanvas *)f1->Get("cHNoise");
  c1->Draw();
  TCanvas *c2 = (TCanvas *)f1->Get("cLNoise");
  c2->Draw();
  TFile *f2 = new TFile("/home/kkbo/beamtest/draw/figureroot/5x5sumwave-" + name);
  TCanvas *c3 = (TCanvas *)f2->Get("cHAmplTime");
  c3->Draw();
  TCanvas *c4 = (TCanvas *)f2->Get("cLAmplTime");
  c4->Draw();
  //TFile *f3 = new TFile("/home/kkbo/beamtest/draw/figureroot/Peak_" + name);
  //TCanvas *c5 = (TCanvas *)f3->Get("c1");
  //c5->Draw();
  //TCanvas *c6 = (TCanvas *)f3->Get("c2");
  //c6->Draw();
}
