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
      f1->SetParameters(100, (h->GetMaximumBin() * h->GetBinWidth(0) + h->GetBinCenter(0)) * a, b, 0.6, 2.8);
      h->Fit(f1, "ESR", "", h->GetBinCenter(1) * 1.5, h->GetBinCenter(h->GetNbinsX()));
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

        h->SetYTitle("Counts");
        h->SetXTitle(xname);
        h->GetYaxis()->SetLabelSize(0.05);
        h->GetYaxis()->SetTitleSize(0.05);
        h->GetXaxis()->SetLabelSize(0.05);
        h->GetXaxis()->SetTitleSize(0.05);
        // h->GetXaxis()->CenterTitle();
        // h->GetYaxis()->CenterTitle();
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
        // leg2->Draw();
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

void PosRes(std::string ecalfile)
{
  gStyle->SetOptFit(1111);
  TFile *InfileECAL = new TFile(ecalfile.data(), "READ");
  TTree *TrECAL = (TTree *)InfileECAL->Get("rec_data");
  vector<int> *SeedID = 0;
  vector<int> *HitID = 0;
  vector<double> *Energy_5x5 = 0;
  vector<double> *Energy_Hit = 0;
  vector<double> *ShowerX = 0;
  vector<double> *ShowerY = 0;
  int triggerID;
  TrECAL->SetBranchAddress("EventID", &triggerID);
  TrECAL->SetBranchAddress("ShowerID", &SeedID);
  TrECAL->SetBranchAddress("ShowerE5x5", &Energy_5x5);
  TrECAL->SetBranchAddress("HitID", &HitID);
  TrECAL->SetBranchAddress("HitEnergy", &Energy_Hit);
  TrECAL->SetBranchAddress("ShowerPosX5x5", &ShowerX);
  TrECAL->SetBranchAddress("ShowerPosY5x5", &ShowerY);

  TH1D *hisx = new TH1D("his1", "posx", 100, -5, 5);
  TH1D *hisy = new TH1D("his2", "posy", 100, -5, 5);
  TH2D *hisecal = new TH2D("his3", "posecal", 50, -5, 5, 50, -5, 5);
  TH2D *histr = new TH2D("his4", "postracker", 50, -5, 5, 50, -5, 5);
  TH2D *hisres = new TH2D("his5", "posres", 50, -5, 5, 50, -5, 5);
  hisx->SetTitle("position resolution;posx[cm];counts");
  hisy->SetTitle("position resolution;posx[cm];counts");
  hisres->SetTitle("position distribution;posx[cm];posy[cm]");
  hisx->SetDirectory(nullptr);
  hisy->SetDirectory(nullptr);
  hisecal->SetDirectory(nullptr);
  histr->SetDirectory(nullptr);
  hisres->SetDirectory(nullptr);

  for (int i = 0; i < TrECAL->GetEntries(); i++)
  {
    TrECAL->GetEntry(i);
    if (ShowerX->size() == 1)
      //   continue;
      hisecal->Fill(ShowerX->at(0), ShowerY->at(0));
  }
  TCanvas *can3 = new TCanvas();
  hisecal->Draw("colz");
  InfileECAL->Close();
}
void PosRes(std::string ecalfile, std::string trackfile)
{
  gStyle->SetOptFit(1111);
  TFile *InfileECAL = new TFile(ecalfile.data(), "READ");
  TTree *TrECAL = (TTree *)InfileECAL->Get("rec_data");
  vector<int> *SeedID = 0;
  vector<int> *HitID = 0;
  vector<double> *Energy_5x5 = 0;
  vector<double> *Energy_Hit = 0;
  vector<double> *ShowerX = 0;
  vector<double> *ShowerY = 0;
  int triggerID;
  TrECAL->SetBranchAddress("EventID", &triggerID);
  TrECAL->SetBranchAddress("ShowerID", &SeedID);
  TrECAL->SetBranchAddress("ShowerE5x5", &Energy_5x5);
  TrECAL->SetBranchAddress("HitID", &HitID);
  TrECAL->SetBranchAddress("HitEnergy", &Energy_Hit);
  TrECAL->SetBranchAddress("ShowerPosX5x5", &ShowerX);
  TrECAL->SetBranchAddress("ShowerPosY5x5", &ShowerY);

  TH1D *hisx = new TH1D("his1", "posx", 100, -5, 5);
  TH1D *hisy = new TH1D("his2", "posy", 100, -5, 5);
  TH2D *hisecal = new TH2D("his3", "posecal", 50, -5, 5, 50, -5, 5);
  TH2D *histr = new TH2D("his4", "postracker", 50, -5, 5, 50, -5, 5);
  TH2D *hisres = new TH2D("his5", "posres", 50, -5, 5, 50, -5, 5);
  hisx->SetTitle("position resolution;posx[cm];counts");
  hisy->SetTitle("position resolution;posx[cm];counts");
  hisres->SetTitle("position distribution;posx[cm];posy[cm]");
  hisx->SetDirectory(nullptr);
  hisy->SetDirectory(nullptr);
  hisecal->SetDirectory(nullptr);
  histr->SetDirectory(nullptr);
  hisres->SetDirectory(nullptr);

  TFile *InfileTrack = new TFile(trackfile.data(), "READ");
  TTree *TrTrack = (TTree *)InfileTrack->Get("Track");
  int trackID;
  double trackPos[3], trackVec[3];
  TrTrack->SetBranchAddress("event", &trackID);
  TrTrack->SetBranchAddress("ecalextraHit", &trackPos);
  TrTrack->SetBranchAddress("ecaltrackVec", &trackVec);
  for (int i = 0; i < std::min(TrECAL->GetEntries(), TrTrack->GetEntries()); i++)
  {
    TrECAL->GetEntry(i);
    TrTrack->GetEntry(i);
    if (!(ShowerX->size() == 1 && Energy_5x5->at(0) > 500 && triggerID == trackID && SeedID->at(0) == 326034 && (trackPos[0] != 0 || trackPos[1] != 0) && (sqrt(Power(trackVec[1], 2) + Power(trackVec[0], 2)) < 0.002)))
      continue;
    if (!(fabs(ShowerX->at(0)) < 1 && fabs(ShowerY->at(0)) < 1))
      continue;
    // if ((trackPos[0] > -20 && trackPos[0] < 0 && trackPos[1] > -10 && trackPos[1] < 10))
    //   continue;
    hisx->Fill(ShowerX->at(0) - trackPos[0] / 10);
    hisy->Fill(ShowerY->at(0) + trackPos[1] / 10);
    hisecal->Fill(ShowerX->at(0), ShowerY->at(0));
    histr->Fill(trackPos[0] / 10, -trackPos[1] / 10);
    hisres->Fill(ShowerX->at(0) - trackPos[0] / 10, ShowerY->at(0) + trackPos[1] / 10);
  }
  TCanvas *can1 = new TCanvas();
  hisx->Draw();
  hisx->Fit("gaus");
  TCanvas *can2 = new TCanvas();
  hisy->Draw();
  hisy->Fit("gaus");
  TCanvas *can3 = new TCanvas();
  hisecal->Draw("colz");
  TCanvas *can4 = new TCanvas();
  histr->Draw("colz");
  TCanvas *can5 = new TCanvas();
  hisres->Draw("colz");
  InfileECAL->Close();
  InfileTrack->Close();
}

void DrawEnergy(string rootfile, double energy = 1000)
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
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

  TChain *t = new TChain("rec_data");
  for (unsigned int i = 0; i < rootlist.size(); i++)
  {
    t->Add(rootlist[i].c_str());
    cout << "Add root file: " << rootlist[i] << endl;
  }

  vector<int> *SeedID = 0;
  vector<int> *HitID = 0;
  vector<double> *Energy_3x3 = 0;
  vector<double> *Energy_5x5 = 0;
  vector<double> *Energy_EAll = 0;
  vector<double> *Energy_Hit = 0;
  vector<double> *ShowerX = 0;
  vector<double> *ShowerY = 0;
  t->SetBranchAddress("ShowerID", &SeedID);
  t->SetBranchAddress("ShowerE3x3", &Energy_3x3);
  t->SetBranchAddress("ShowerE5x5", &Energy_5x5);
  t->SetBranchAddress("ShowerEAll", &Energy_EAll);
  t->SetBranchAddress("HitID", &HitID);
  t->SetBranchAddress("HitEnergy", &Energy_Hit);
  t->SetBranchAddress("ShowerPosX5x5", &ShowerX);
  t->SetBranchAddress("ShowerPosY5x5", &ShowerY);

  TH2F *hpos = new TH2F("hpos", "Shower Position", 50, -5, 5, 50, 5, 5);
  TH1F *hposx = new TH1F("hposx", "Shower X Position", 100, -12.5, 12.5);
  TH1F *hposy = new TH1F("hposy", "Shower Y Position", 100, -12.5, 12.5);
  // determine electron beam energy
  TH1F *energy_test = new TH1F("energy", "energy", 1000, 0, 10000);
  double maxenergy = 0, maxheight = 0;
  for (int i = 0; i < t->GetEntries(); i++)
  {
    t->GetEntry(i);
    for (size_t j = 0; j < Energy_EAll->size(); j++)
    {
      energy_test->Fill(Energy_EAll->at(j));
      if ((Energy_EAll->at(j) > maxenergy) && (energy_test->GetBinContent(static_cast<int>(Energy_EAll->at(j) / energy_test->GetBinWidth(0))) > 50))
        maxenergy = Energy_EAll->at(j);
    }
  }
  int startbin = 0, maxbin = 0;
  startbin = maxenergy / 2 / 10;
  for (int i = startbin; i < energy_test->GetNbinsX(); i++)
  {
    if (energy_test->GetBinContent(i) > maxheight)
    {
      maxheight = energy_test->GetBinContent(i);
      maxbin = i;
    }
  }
  energy = std::round(maxbin * energy_test->GetBinWidth(0) / 100) * 100;
  string energy_str = to_string(int(energy)) + "MeV";
  double low = 0.5 * energy / 1000;
  double high = 1.2 * energy / 1000;
  double seedcut = 0.2 * energy / 1000;
  int binnum = 200;
  TH1F *henergy_ecal = new TH1F("henergy_ecal", "ECAL Energy Distribution", binnum, low, high);
  double perbin = (high - low) / binnum;
  for (int i = 0; i < t->GetEntries(); i++)
  {
    t->GetEntry(i);
    for (unsigned int j = 0; j < SeedID->size(); j++)
    {
      if (SeedID->at(j) == 326034)
      {
        // center crystal ID for ECAL
        double seed_energy = -1;
        int hitnum = 0;
        for (unsigned int k = 0; k < HitID->size(); k++)
        {
          if (HitID->at(k) == 326034)
          {
            seed_energy = Energy_Hit->at(k) / 1000;
          }
          else if (Energy_Hit->at(k) > 3)
          {
            hitnum++;
          }
        }

        if (seed_energy < seedcut)
          continue;
        if (hitnum < 2)
          continue;
        // select those events hitting the central area
        int poslimit = 2;
        // if (!(ShowerX->at(j) > -poslimit && ShowerX->at(j) < poslimit && ShowerY->at(j) < poslimit && ShowerY->at(j) > -poslimit))
        //   continue;

        // henergy_ecal->Fill(Energy_3x3->at(j) / 1000);
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
  // c_pos->SaveAs(Form("%s.png", pos_name.Data()));
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
  // c_posx->SaveAs(Form("%s.png", posx_name.Data()));
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
  // c_posy->SaveAs(Form("%s.png", posy_name.Data()));

  TString canvas_name = "c_" + energy_str;
  TCanvas *c = new TCanvas(canvas_name, canvas_name, 800, 600);
  TF1 *f1 = new TF1("f1", "crystalball", low * 1.01, high * 0.95);
  f1->SetNpx(1000);

  double mean;
  double sigma;
  std::vector<double> Vfit = DoFit(henergy_ecal, f1, c, "Energy/GeV", perbin);
  mean = Vfit[0];
  sigma = Vfit[1];
  c->SaveAs(Form("%s.png", canvas_name.Data()));
  cout << t->GetEntries() << " entries in total." << endl;
  cout << "Event entries selected: " << henergy_ecal->Integral() << endl;
  cout << "Energy: " << energy_str << ", Mean: " << mean * 1000 << " MeV, Resolution: " << sigma << " %" << endl;
  cout << "Energy fit error: " << f1->GetParError(1) << std::endl;
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
    DrawEnergy(argv[1], 1000);
  }
  else
  {
    DrawEnergy(argv[1], std::stod(argv[2]));
  }
  return 0;
}
