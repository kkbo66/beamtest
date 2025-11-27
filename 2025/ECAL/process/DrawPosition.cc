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
#include <TLatex.h>
using namespace TMath;
using namespace std;

void DrawPosition(std::string ecalfile, std::string trackfile)
{
    gStyle->SetOptStat(0);
    // gStyle->SetOptFit(1111);
    gStyle->SetOptTitle(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetTitleFont(42, "xyz");
    gStyle->SetLabelFont(42, "xyz");
    gStyle->SetLegendFont(42);
    gStyle->SetTextFont(42);
    gStyle->SetTitleSize(0.05, "xyz");
    gStyle->SetLabelSize(0.04, "xyz");
    gStyle->SetLegendTextSize(0.06);
    gStyle->SetTitleOffset(1.0, "y");
    gStyle->SetTitleOffset(0.8, "x");
    gStyle->SetPadGridX(true);
    gStyle->SetPadGridY(true);

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

    // determine electron beam energy
    double energy;
    TH1F *energy_test = new TH1F("energy", "energy", 1000, 0, 10000);
    double maxenergy = 0, maxheight = 0;
    for (int i = 0; i < TrECAL->GetEntries(); i++)
    {
        TrECAL->GetEntry(i);
        for (size_t j = 0; j < Energy_5x5->size(); j++)
        {
            energy_test->Fill(Energy_5x5->at(j));
            if ((Energy_5x5->at(j) > maxenergy) && (energy_test->GetBinContent(static_cast<int>(Energy_5x5->at(j) / energy_test->GetBinWidth(0))) > 50))
                maxenergy = Energy_5x5->at(j);
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
    double energy_cut = energy / 5;

    TH1D *hisx = new TH1D("his1", "posx", 100, -40, 40);
    TH1D *hisy = new TH1D("his2", "posy", 100, -40, 40);
    TH2D *hisecal = new TH2D("his3", "posecal", 40, -40, 40, 40, -40, 40);
    TH2D *histr = new TH2D("his4", "postracker", 40, -40, 40, 40, -40, 40);
    TH2D *hisres = new TH2D("his5", "posres", 40, -40, 40, 40, -40, 40);
    hisx->SetTitle("position resolution;#Deltax[mm];counts");
    hisy->SetTitle("position resolution;#Deltax[mm];counts");
    hisres->SetTitle("position distribution;posx[mm];posy[mm]");
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
        if (!(ShowerX->size() == 1 && Energy_5x5->at(0) > energy_cut && triggerID == trackID && SeedID->at(0) == 326034 && (trackPos[0] != 0 || trackPos[1] != 0) && (sqrt(Power(trackVec[1], 2) + Power(trackVec[0], 2)) < 0.002)))
            continue;
        if (!(fabs(ShowerX->at(0)) < 1 && fabs(ShowerY->at(0)) < 1))
            continue;
        // if ((trackPos[0] > -20 && trackPos[0] < 0 && trackPos[1] > -10 && trackPos[1] < 10))
        //   continue;
        hisx->Fill(ShowerX->at(0) * 10 - trackPos[0]);
        hisy->Fill(ShowerY->at(0) * 10 + trackPos[1]);
        hisecal->Fill(ShowerX->at(0) * 10, ShowerY->at(0) * 10);
        histr->Fill(trackPos[0], -trackPos[1]);
        hisres->Fill(ShowerX->at(0) * 10 - trackPos[0], ShowerY->at(0) * 10 + trackPos[1]);
    }
    TLatex *tex = new TLatex();
    tex->SetTextAlign(20);
    tex->SetTextFont(42);
    tex->SetTextSize(0.08);
    tex->SetTextColor(kRed);

    TCanvas *can1 = new TCanvas();
    hisx->Draw();
    hisx->Fit("gaus");
    TCanvas *can2 = new TCanvas();
    hisy->Draw();
    hisy->Fit("gaus");
    tex->DrawLatexNDC(0.3, 0.8, Form("#sigma=%.2f mm", hisy->GetFunction("gaus")->GetParameter(2)));
    TCanvas *can3 = new TCanvas();
    hisecal->Draw("colz");
    TCanvas *can4 = new TCanvas();
    histr->Draw("colz");
    TCanvas *can5 = new TCanvas();
    hisres->Draw("colz");
    InfileECAL->Close();
    InfileTrack->Close();
}
void DrawPosition(TString path)
{
    std::string ecalfile = Form("%s/rec.root", path.Data());
    std::string trackfile = Form("%s/Tracker-step4-rec.root", path.Data());
    gStyle->SetOptStat(0);
    // gStyle->SetOptFit(1111);
    gStyle->SetOptTitle(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetTitleFont(42, "xyz");
    gStyle->SetLabelFont(42, "xyz");
    gStyle->SetLegendFont(42);
    gStyle->SetTextFont(42);
    gStyle->SetTitleSize(0.05, "xyz");
    gStyle->SetLabelSize(0.04, "xyz");
    gStyle->SetLegendTextSize(0.06);
    gStyle->SetTitleOffset(1.0, "y");
    gStyle->SetTitleOffset(0.8, "x");
    gStyle->SetPadGridX(true);
    gStyle->SetPadGridY(true);

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

    // determine electron beam energy
    double energy;
    TH1F *energy_test = new TH1F("energy", "energy", 1000, 0, 10000);
    double maxenergy = 0, maxheight = 0;
    for (int i = 0; i < TrECAL->GetEntries(); i++)
    {
        TrECAL->GetEntry(i);
        for (size_t j = 0; j < Energy_5x5->size(); j++)
        {
            energy_test->Fill(Energy_5x5->at(j));
            if ((Energy_5x5->at(j) > maxenergy) && (energy_test->GetBinContent(static_cast<int>(Energy_5x5->at(j) / energy_test->GetBinWidth(0))) > 50))
                maxenergy = Energy_5x5->at(j);
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
    double energy_cut = energy / 5;

    TH1D *hisx = new TH1D("his1", "posx", 100, -40, 40);
    TH1D *hisy = new TH1D("his2", "posy", 100, -40, 40);
    TH2D *hisecal = new TH2D("his3", "posecal", 40, -40, 40, 40, -40, 40);
    TH2D *histr = new TH2D("his4", "postracker", 40, -40, 40, 40, -40, 40);
    TH2D *hisres = new TH2D("his5", "posres", 40, -40, 40, 40, -40, 40);
    hisx->SetTitle("position resolution;#Deltax[mm];counts");
    hisy->SetTitle("position resolution;#Deltax[mm];counts");
    hisres->SetTitle("position distribution;posx[mm];posy[mm]");
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
        if (!(ShowerX->size() == 1 && Energy_5x5->at(0) > energy_cut && triggerID == trackID && SeedID->at(0) == 326034 && (trackPos[0] != 0 || trackPos[1] != 0) && (sqrt(Power(trackVec[1], 2) + Power(trackVec[0], 2)) < 0.002)))
            continue;
        if (!(fabs(ShowerX->at(0)) < 1.5 && fabs(ShowerY->at(0)) < 1.5))
            continue;
        // if ((trackPos[0] > -20 && trackPos[0] < 0 && trackPos[1] > -10 && trackPos[1] < 10))
        //   continue;
        hisx->Fill(ShowerX->at(0) * 10 - trackPos[0]);
        hisy->Fill(ShowerY->at(0) * 10 + trackPos[1]);
        hisecal->Fill(ShowerX->at(0) * 10, ShowerY->at(0) * 10);
        histr->Fill(trackPos[0], -trackPos[1]);
        hisres->Fill(ShowerX->at(0) * 10 - trackPos[0], ShowerY->at(0) * 10 + trackPos[1]);
    }
    TLatex *tex = new TLatex();
    tex->SetTextAlign(20);
    tex->SetTextFont(42);
    tex->SetTextSize(0.08);
    tex->SetTextColor(kRed);

    TCanvas *can1 = new TCanvas();
    hisx->Draw();
    hisx->Fit("gaus");
    TCanvas *can2 = new TCanvas();
    hisy->Draw();
    hisy->Fit("gaus");
    tex->DrawLatexNDC(0.3, 0.8, Form("#sigma=%.2f mm", hisy->GetFunction("gaus")->GetParameter(2)));
    can2->SaveAs(Form("%s/posresolution_filterXY.png", path.Data()));
    can2->SaveAs(Form("%s/posresolution_filterXY.pdf", path.Data()));
    // can2->SaveAs(Form("%s/posresolution_nofilterXY.png", path.Data()));
    // can2->SaveAs(Form("%s/posresolution_nofilterXY.pdf", path.Data()));
    TCanvas *can3 = new TCanvas();
    hisecal->Draw("colz");
    TCanvas *can4 = new TCanvas();
    histr->Draw("colz");
    TCanvas *can5 = new TCanvas();
    hisres->Draw("colz");
    InfileECAL->Close();
    InfileTrack->Close();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "parameters too few, parameter1: reconstructed ECAL file; parameter2: Tracker file" << std::endl;
        std::cerr << "or, parameters1: data path('rec.root' and 'Tracker-step4-rec.root')";
        return 1;
    }
    std::string ecalfile, trackerfile;
    if (argc == 2)
    {
        DrawPosition(argv[1]);
    }
    else if (argc == 3)
    {
        ecalfile = argv[1];
        trackerfile = argv[2];
        DrawPosition(ecalfile, trackerfile);
    }
    else
    {
        std::cerr << "too many parameters!" << std::endl;
        return 1;
    }
    return 0;
}
