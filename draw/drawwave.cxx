using namespace std;
using namespace TMath;
void drawwave()
{

  gROOT->SetBatch(kTRUE);

  TFile *f = new TFile("/home/kkbo/beamtest/2025/ECAL/build/re.root");
  TTree *t = (TTree *)f->Get("decode_data");

  TTreeReader myReader(t);
  TTreeReaderArray<Double_t> LAmp(myReader, "Hit_4_3.LAmplitude");
  TTreeReaderArray<Double_t> HAmp(myReader, "Hit_4_3.HAmplitude");

  int n = 0;
  while (myReader.Next())
  {
    if (n > 19)
      break;
    // const Double_t *la = LAmp.Get();
    // const Double_t *ha = HAmp.Get();

    TCanvas *c = new TCanvas("c", "c", 800, 600);
    c->SetGrid();
    TGraph *gla = new TGraph();
    for (int i = 0; i < 256; i++)
    {
      // gla->SetPoint(i, i, la[i]);
      gla->SetPoint(i, i * 12.5, LAmp[i]);
    }
    gla->SetLineColor(kBlue);
    gla->SetLineWidth(2);
    gla->GetXaxis()->SetTitle("Time (ns)");
    gla->GetYaxis()->SetTitle("ADC");
    gla->Draw("AL");
    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->AddEntry(gla, "LAmp", "l");
    leg->Draw();
    c->Print(Form("pdf/lAmp_%d.pdf", n));

    TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
    c2->SetGrid();
    TGraph *gha = new TGraph();
    for (int i = 0; i < 256; i++)
    {
      gha->SetPoint(i, i * 12.5, HAmp[i]);
    }
    gha->SetLineColor(kRed);
    gha->SetLineWidth(2);
    gha->GetXaxis()->SetTitle("Time (ns)");
    gha->GetYaxis()->SetTitle("ADC");
    gha->Draw("AL");
    TLegend *leg2 = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg2->SetFillColor(0);
    leg2->SetFillStyle(0);
    leg2->SetBorderSize(0);
    leg2->AddEntry(gha, "HAmp", "l");
    leg2->Draw();
    c2->Print(Form("pdf/hAmp_%d.pdf", n));

    delete c;
    delete c2;

    n++;
  }
}
