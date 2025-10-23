#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLine.h>
#include <TF1.h>
#include <TMath.h>
#include <TString.h>
#include <TAxis.h>
#include <TPaveStats.h>
#include <TChain.h>
using namespace TMath;
using namespace std;

std::vector<double> DoFit(TH1F *h, TF1 *f1, TCanvas *c, TString xname, double perbin)
{
  c->cd();
  gPad->SetGrid();
  bool swith = false;
  double mean = 0;
  double sigma = 0;
  std::vector<double> Vout;
  Vout.clear();
  for (int i = 0; i < 16; i++)
  {
    double a = 0.98 + 0.01 * double(i + 1);
    for (int j = 0; j < 16; j++)
    {
      double b = 0.030 + 0.0001 * double(j);
      f1->SetParameters(100, h->GetMean() * a, b, 0.6, 2.8);
      // f1->FixParameter(2,1.4e-02);
      h->Fit(f1, "ESR");
      double Mean1 = f1->GetParameter(1);
      double Sigma1 = f1->GetParameter(2);
      double A1 = abs(f1->GetParameter(3));
      double N1 = f1->GetParameter(4);
      double right1 = Sigma1 * sqrt(log(4.0));
      double left1 = Sigma1 * ((pow(2 * exp(-0.5 * A1 * A1), 1 / N1) - 1) * N1 / A1 + A1);
      double a1 = (sqrt(log(4.0)) + A1 + (pow(2 * exp(-0.5 * A1 * A1), 1 / N1) - 1) * N1 / A1) / Mean1 / (2 * sqrt(log(4.0))) * 100;
      double b1 = -(Sigma1 * sqrt(log(4.0)) + Sigma1 * ((pow(2 * exp(-0.5 * A1 * A1), 1 / N1) - 1) * N1 / A1 + A1)) / Mean1 / Mean1 / (2 * sqrt(log(4.0))) * 100;
      double c1 = Sigma1 * (1 - pow(2 * exp(-0.5 * A1 * A1), 1 / N1) - (pow(2 * exp(-0.5 * A1 * A1), 1 / N1) - 1) * N1 / A1 / A1) / Mean1 / (2 * sqrt(log(4.0))) * 100;
      double d1 = Sigma1 * ((pow(2 * exp(-0.5 * A1 * A1), 1 / N1) - 1) / A1 - pow(2 * exp(-0.5 * A1 * A1), 1 / N1) * (log(2.0) + log(exp(-0.5 * A1 * A1))) / N1 / A1) / Mean1 / (2 * sqrt(log(4.0))) * 100;
      double Error1 = a1 * a1 * f1->GetParError(2) * f1->GetParError(2) + b1 * b1 * f1->GetParError(1) * f1->GetParError(1) + c1 * c1 * f1->GetParError(3) * f1->GetParError(3) + d1 * d1 * f1->GetParError(4) * f1->GetParError(4);
      if (sqrt(Error1) > 0 && sqrt(Error1) < 5 && (left1 + right1) > 0)
        swith = true;
      if (swith == true)
      {
        TPaveText *pt11 = new TPaveText(0.4, 0.8, 0.4, 0.8, "BRNDC");
        TPaveText *pt21 = new TPaveText(0.4, 0.7, 0.4, 0.7, "BRNDC");
        TPaveText *pt31 = new TPaveText(0.4, 0.6, 0.4, 0.6, "BRNDC");
        TPaveText *pt41 = new TPaveText(0.4, 0.5, 0.4, 0.5, "BRNDC");
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
        TString p11 = Form("%.4f", f1->GetParameter(1));
        TString s11 = "Mean=" + p11 + "GeV";

        TString p21 = Form("%.3f", (left1 + right1) / (2 * sqrt(log(4.0))) * 1000);
        TString s21 = "Sigma=" + p21 + "MeV";
        TString p31 = Form("%.2f", ((left1 + right1) / (2 * sqrt(log(4.0)))) / Mean1 * 100);
        TString p311 = Form("%.2f", sqrt(Error1));
        TString s31 = "Sigma/Mean=" + p31 + "%";
        mean = f1->GetParameter(1);
        sigma = ((left1 + right1) / (2 * sqrt(log(4.0)))) / Mean1 * 100;

        TText *text1;
        text1 = pt11->AddText(s11);
        text1 = pt21->AddText(s21);
        text1 = pt31->AddText(s31);

        h->SetYTitle(Form("Events /%.2f(MeV/c^{2})", 1000 * perbin));
        h->SetXTitle(xname);
        h->GetYaxis()->SetLabelSize(0.05);
        h->GetYaxis()->SetTitleSize(0.05);
        h->GetXaxis()->SetLabelSize(0.05);
        h->GetXaxis()->SetTitleSize(0.05);
        h->GetXaxis()->SetTitleOffset(0.9);
        h->GetYaxis()->SetTitleOffset(0.9);

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
        leg2->AddEntry(h, label, "l");
        leg2->Draw();
      }
      if (swith == true)
        break;
    }
    if (swith == true)
      break;
  }
  Vout.push_back(mean);
  Vout.push_back(sigma);
  return Vout;
}

void DrawPosEnergy(string rootfile, double energy = 1000)
{

  gStyle->SetOptStat(0);
  vector<string> rootlist;
  rootlist.clear();
  if (rootfile.find(".txt") != string::npos)
  {
    ifstream infile;
    infile.open(rootfile.c_str());
    string line;
    while (getline(infile, line))
    {
      rootlist.push_back(line);
    }
  }
  else if (rootfile.find(".root") != string::npos)
  {
    rootlist.push_back(rootfile);
  }
  else
  {
    cout << "Please input root file" << endl;
    return;
  }

  string energy_str = to_string(int(energy)) + "MeV";

  TChain *t = new TChain("rec_data");
  for (unsigned int i = 0; i < rootlist.size(); i++)
  {
    t->Add(rootlist[i].c_str());
    cout << "Add root file: " << rootlist[i] << endl;
  }

  vector<int> *SeedID = 0;
  vector<int> *HitID = 0;
  vector<double> *Energy_5x5 = 0;
  vector<double> *Energy_Hit = 0;
  vector<double> *ShowerX = 0;
  vector<double> *ShowerY = 0;
  t->SetBranchAddress("ShowerID", &SeedID);
  t->SetBranchAddress("ShowerE5x5", &Energy_5x5);
  t->SetBranchAddress("HitID", &HitID);
  t->SetBranchAddress("HitEnergy", &Energy_Hit);
  t->SetBranchAddress("ShowerPosX5x5", &ShowerX);
  t->SetBranchAddress("ShowerPosY5x5", &ShowerY);

  TH2F *hpos = new TH2F("hpos", "Shower Position", 50, -5, 5, 50, 5, 5);
  TH1F *hposx = new TH1F("hposx", "Shower X Position", 100, -12.5, 12.5);
  TH1F *hposy = new TH1F("hposy", "Shower Y Position", 100, -12.5, 12.5);

  double low = 0.5 * energy / 1000;
  // double high = 1.05*energy/1000;
  double high = 1.2 * energy / 1000;
  double seedcut = 0.2 * energy / 1000;
  TH1F *henergy_ecal = new TH1F("henergy_ecal", "ECAL Energy Distribution", 100, low, high);
  double perbin = (high - low) / 100.0;
  cout << t->GetEntries() << " entries in total." << endl;
  for (int i = 0; i < t->GetEntries(); i++)
  {
    t->GetEntry(i);
    for (unsigned int j = 0; j < SeedID->size(); j++)
    {
      if (SeedID->at(j) == 326034)
      { // center crystal ID for ECAL
        double seed_energy = -1;
        int hitnum = 0;
        for (unsigned int k = 0; k < HitID->size(); k++)
        {
          if (HitID->at(k) == 326034)
          {
            seed_energy = Energy_Hit->at(k) / 1000;
            // break;
          }
          else if (Energy_Hit->at(k) > 5)
          {
            hitnum++;
          }
        }
        // cout<<"Event: "<<i<<", Seed Energy: "<<seed_energy*1000<<" MeV"<< ", E5x5: "<<Energy_5x5->at(j)*1000<<" MeV"<<endl;
        if (seed_energy < seedcut)
          continue;
        if (hitnum < 2)
          continue;
        // select those events hitting the central area
        if (!(ShowerX->at(j) > -1 && ShowerX->at(j) < 1 && ShowerY->at(j) < 1 && ShowerY->at(j) > -1))
          continue;
        henergy_ecal->Fill(Energy_5x5->at(j) / 1000);
        hpos->Fill(ShowerX->at(j), ShowerY->at(j));
        hposx->Fill(ShowerX->at(j));
        hposy->Fill(ShowerY->at(j));
      }
    }
  }

  TString pos_name = "c_pos_" + energy_str;
  TCanvas *c_pos = new TCanvas(pos_name, pos_name, 800, 600);
  c_pos->cd();
  gPad->SetGrid();
  hpos->SetXTitle("Shower X Position (cm)");
  hpos->SetYTitle("Shower Y Position (cm)");
  hpos->GetXaxis()->SetLabelSize(0.05);
  hpos->GetXaxis()->SetTitleSize(0.05);
  hpos->GetYaxis()->SetLabelSize(0.05);
  hpos->GetYaxis()->SetTitleSize(0.05);
  hpos->GetXaxis()->SetTitleOffset(0.9);
  hpos->GetYaxis()->SetTitleOffset(0.9);
  hpos->SetLineColor(kBlue);
  hpos->SetLineWidth(2);
  hpos->Draw("colz");
  c_pos->SaveAs(Form("%s.png", pos_name.Data()));
  TString posx_name = "c_posx_" + energy_str;
  TCanvas *c_posx = new TCanvas(posx_name, posx_name, 800, 600);
  c_posx->cd();
  gPad->SetGrid();
  hposx->SetXTitle("Shower X Position (cm)");
  hposx->SetYTitle("Entries");
  hposx->GetXaxis()->SetLabelSize(0.05);
  hposx->GetXaxis()->SetTitleSize(0.05);
  hposx->GetYaxis()->SetLabelSize(0.05);
  hposx->GetYaxis()->SetTitleSize(0.05);
  hposx->GetXaxis()->SetTitleOffset(0.9);
  hposx->GetYaxis()->SetTitleOffset(0.9);
  hposx->SetLineColor(kBlue);
  hposx->SetLineWidth(2);
  hposx->Draw("hist");
  c_posx->SaveAs(Form("%s.png", posx_name.Data()));
  TString posy_name = "c_posy_" + energy_str;
  TCanvas *c_posy = new TCanvas(posy_name, posy_name, 800, 600);
  c_posy->cd();
  gPad->SetGrid();
  hposy->SetXTitle("Shower Y Position (cm)");
  hposy->SetYTitle("Entries");
  hposy->GetXaxis()->SetLabelSize(0.05);
  hposy->GetXaxis()->SetTitleSize(0.05);
  hposy->GetYaxis()->SetLabelSize(0.05);
  hposy->GetYaxis()->SetTitleSize(0.05);
  hposy->GetXaxis()->SetTitleOffset(0.9);
  hposy->GetYaxis()->SetTitleOffset(0.9);
  hposy->SetLineColor(kBlue);
  hposy->SetLineWidth(2);
  hposy->Draw("hist");
  c_posy->SaveAs(Form("%s.png", posy_name.Data()));

  TString canvas_name = "c_" + energy_str;
  TCanvas *c = new TCanvas(canvas_name, canvas_name, 800, 600);
  TF1 *f1 = new TF1("f1", "crystalball", low * 1.01, high * 0.95);
  f1->SetNpx(1000);

  double mean;
  double sigma;
  std::vector<double> Vfit = DoFit(henergy_ecal, f1, c, "E_{5x5} (GeV)", perbin);
  mean = Vfit[0];
  sigma = Vfit[1];
  c->SaveAs(Form("%s.png", canvas_name.Data()));
  cout << "Event entries: " << henergy_ecal->Integral() << endl;
  cout << "Energy: " << energy_str << ", Mean: " << mean * 1000 << " MeV, Resolution: " << sigma << " %" << endl;
}
int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    std::cerr << "parameters too few, parameter1: reconstructed root file; parameter2: seed energy cut" << std::endl;
    return 1;
  }
  else if (argc == 2)
  {
    DrawPosEnergy(argv[1], 1000);
  }
  else
  {
    DrawPosEnergy(argv[1], std::stod(argv[2]));
  }
  return 0;
}
