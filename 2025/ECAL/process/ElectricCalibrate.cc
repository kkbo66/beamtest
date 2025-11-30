#include <iostream>
#include <fstream>
#include <string>

#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1.h"
#include "TPaveStats.h"
#include "TF1.h"
#include "TGraphErrors.h"

// #include "stcfplotstyle.c"
// Convert channelID (0-24) to canvasID (1-25)
int ch2can(int channel)
{
    int row, column;
    row = (4 - channel % 5), column = channel / 5;
    return 5 * row + column + 1;
}
// Convert hitID (1_1 - 5_5) to channelID (0-24)
int hit2ch(int hitx, int hity)
{
    return (hitx - 1) * 5 + (hity - 1);
}
// Make struct type for decode_data
void makeclass(TString filename)
{
    TFile *infile = new TFile(filename.Data(), "READ");
    TTree *tr = (TTree *)infile->Get("decode_data");
    tr->MakeClass("DecodeClass");
}
// Input decode calibration root file, draw and fit energy spctra of H/LG
void spectrum_fit(TString filename, TString storefile = "")
{
    // SetPrelimStyle();
    // SetStyle();
    gStyle->SetOptFit(0110);
    gStyle->SetOptStat(0100);
    TFile *infile = new TFile(filename.Data(), "READ");
    TTree *tr = (TTree *)infile->Get("decode_data");
    TLeaf *lfLGped[25], *lfLGpeak[25], *lfHGped[25], *lfHGpeak[25];
    double hisminLG[25], hismaxLG[25], hisminHG[25], hismaxHG[25];
    double pedLG[25], peakLG[25], pedHG[25], peakHG[25];
    double tempLG, tempHG;
    memset(hisminLG, 0, sizeof(hismaxHG));
    memset(hismaxLG, 0, sizeof(hismaxLG));
    memset(hisminHG, 0, sizeof(hisminHG));
    memset(hismaxHG, 0, sizeof(hismaxHG));
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            lfLGped[5 * i + j] = tr->GetLeaf(Form("Hit_%d_%d.LowGainPedestal", i + 1, j + 1));
            lfHGped[5 * i + j] = tr->GetLeaf(Form("Hit_%d_%d.HighGainPedestal", i + 1, j + 1));
            lfLGpeak[5 * i + j] = tr->GetLeaf(Form("Hit_%d_%d.LowGainPeak", i + 1, j + 1));
            lfHGpeak[5 * i + j] = tr->GetLeaf(Form("Hit_%d_%d.HighGainPeak", i + 1, j + 1));
        }
    {
        tr->GetEntry(0);
        for (int j = 0; j < 25; j++)
        {
            peakLG[j] = lfLGpeak[j]->GetValue();
            peakHG[j] = lfHGpeak[j]->GetValue();
            pedLG[j] = lfLGped[j]->GetValue();
            pedHG[j] = lfHGped[j]->GetValue();
            tempLG = peakLG[j] - pedLG[j];
            tempHG = peakHG[j] - pedHG[j];
            hisminLG[j] = tempLG;
            hismaxLG[j] = tempLG;
            hisminHG[j] = tempHG;
            hismaxHG[j] = tempHG;
        }
    }
    for (int i = 0; i < tr->GetEntries(); i++)
    {
        tr->GetEntry(i);
        for (int j = 0; j < 25; j++)
        {
            peakLG[j] = lfLGpeak[j]->GetValue();
            peakHG[j] = lfHGpeak[j]->GetValue();
            pedLG[j] = lfLGped[j]->GetValue();
            pedHG[j] = lfHGped[j]->GetValue();
            tempLG = peakLG[j] - pedLG[j];
            tempHG = peakHG[j] - pedHG[j];
            if (tempLG < hisminLG[j])
                hisminLG[j] = tempLG;
            else if (tempLG > hismaxLG[j])
                hismaxLG[j] = tempLG;
            if (tempHG < hisminHG[j])
                hisminHG[j] = tempHG;
            else if (tempHG > hismaxHG[j])
                hismaxHG[j] = tempHG;
        }
    }
    TH1F *hisLG[25], *hisHG[25];
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            hisLG[5 * i + j] = new TH1F(Form("Hit_%d_%d_LGSpec", i + 1, j + 1), ";ADC value;counts", static_cast<int>(1.2 * hismaxLG[5 * i + j] - 0.8 * hisminLG[5 * i + j]), 0.8 * hisminLG[5 * i + j], 1.2 * hismaxLG[5 * i + j]);
            hisHG[5 * i + j] = new TH1F(Form("Hit_%d_%d_HGSpec", i + 1, j + 1), ";ADC value;counts", static_cast<int>(1.2 * hismaxHG[5 * i + j] - 0.8 * hisminHG[5 * i + j]), 0.8 * hisminHG[5 * i + j], 1.2 * hismaxHG[5 * i + j]);
            hisLG[5 * i + j]->SetDirectory(0);
            hisHG[5 * i + j]->SetDirectory(0);
        }
    for (int i = 0; i < tr->GetEntries(); i++)
    {
        tr->GetEntry(i);
        for (int j = 0; j < 25; j++)
        {
            peakLG[j] = lfLGpeak[j]->GetValue();
            peakHG[j] = lfHGpeak[j]->GetValue();
            pedLG[j] = lfLGped[j]->GetValue();
            pedHG[j] = lfHGped[j]->GetValue();
            hisLG[j]->Fill(peakLG[j] - pedLG[j]);
            hisHG[j]->Fill(peakHG[j] - pedHG[j]);
        }
    }
    TCanvas *canLG = new TCanvas("canLG", "canLG", 1600, 900);
    TCanvas *canHG = new TCanvas("canHG", "canHG", 1600, 900);
    TPaveStats *pave;
    canLG->Divide(5, 5);
    canHG->Divide(5, 5);
    for (int i = 0; i < 25; i++)
    {
        canLG->cd(ch2can(i));
        hisLG[i]->Draw();
        hisLG[i]->Fit("gaus", "QR", "", hisLG[i]->GetXaxis()->GetXmin(), hisLG[i]->GetXaxis()->GetXmax());
        gPad->Update();
        pave = (TPaveStats *)hisLG[i]->FindObject("stats");
        pave->SetX1NDC(0.5);
        pave->SetX2NDC(0.9);
        pave->SetY1NDC(0.5);
        pave->SetY2NDC(0.9);
        pave->SetFillStyle(0);
        pave->SetTextColor(kRed);
        gPad->Modified();
        gPad->Update();

        canHG->cd(ch2can(i));
        hisHG[i]->Draw();
        hisHG[i]->Fit("gaus", "QR", "", hisHG[i]->GetXaxis()->GetXmin(), hisHG[i]->GetXaxis()->GetXmax());
        gPad->Update();
        pave = (TPaveStats *)hisHG[i]->FindObject("stats");
        pave->SetX1NDC(0.5);
        pave->SetX2NDC(0.9);
        pave->SetY1NDC(0.5);
        pave->SetY2NDC(0.9);
        pave->SetFillStyle(0);
        pave->SetTextColor(kRed);
        gPad->Modified();
        gPad->Update();
    }
    if (storefile.Sizeof() == 1)
        storefile = filename.Remove(filename.Last('.')).Append("_spec.root");
    TFile *outfile = new TFile(storefile.Data(), "RECREATE");
    outfile->cd();
    canLG->Write();
    canHG->Write();
    for (int i = 0; i < 25; i++)
    {
        hisLG[i]->Write();
        hisHG[i]->Write();
    }

    outfile->Close();
    infile->Close();
    delete canHG;
    delete canLG;
}
// Retrieve fit parameter from stored spec.root (0-24 LG, 25-49 HG)
void parameter_get(TString filename, double mean[50], double mean_error[50])
{
    TFile *infile = new TFile(filename.Data(), "READ");
    TH1F *his;
    TF1 *fun;
    for (int i = 1; i < 6; i++)
        for (int j = 1; j < 6; j++)
        {
            his = (TH1F *)infile->Get(Form("Hit_%d_%d_LGSpec", i, j));
            fun = his->GetFunction("gaus");
            mean[hit2ch(i, j)] = fun->GetParameter(1);
            mean_error[hit2ch(i, j)] = fun->GetParError(1);

            his = (TH1F *)infile->Get(Form("Hit_%d_%d_HGSpec", i, j));
            fun = his->GetFunction("gaus");
            mean[hit2ch(i, j) + 25] = fun->GetParameter(1);
            mean_error[hit2ch(i, j) + 25] = fun->GetParError(1);
        }
    infile->Close();
}
// Retrieve calibration parameter from calibration (0-24 LG)
void calipars_get(TString filename, double calpar[50])
{
    TFile *infile = new TFile(filename.Data(), "READ");
    TGraphErrors *gr;
    TF1 *fun;
    for (int i = 1; i < 6; i++)
        for (int j = 1; j < 6; j++)
        {
            gr = (TGraphErrors *)infile->Get(Form("Hit_%d_%d_LGCal", i, j));
            fun = gr->GetFunction("pol1");
            calpar[hit2ch(i, j)] = fun->GetParameter(1);
            gr = (TGraphErrors *)infile->Get(Form("Hit_%d_%d_HGCal", i, j));
            fun = gr->GetFunction("pol1");
            calpar[hit2ch(i, j) + 25] = fun->GetParameter(1);
        }
    infile->Close();
}
// Input two parameters: 1.filelist.txt 2.chargelist.txt
int main(int argc, char *argv[])
{
    gStyle->SetOptFit(1111);
    if (argc != 3)
    {
        std::cerr << "Input incorrect parameters!" << std::endl;
        std::cout << "Please input ./electric_calibrate + par[0](filelist.txt) + par[1](chargelist.txt)" << std::endl;
        return 1;
    }
    std::string filename1 = argv[1], filename2 = argv[2];
    std::vector<std::string> filenamelist;
    std::vector<double> chargelist;
    std::string temp1, temp2;
    std::ifstream file1(filename1.data());
    std::ifstream file2(filename2.data());
    while (!file1.eof() && !file2.eof())
    {
        file1 >> temp1;
        file2 >> temp2;
        filenamelist.push_back(temp1);
        chargelist.push_back(std::stod(temp2) * 4);
    }
    assert(filenamelist.size() == chargelist.size());
    double mean[50], error[50];
    std::array<double, 50> arraytemp;
    std::vector<std::array<double, 50>> matrixM;
    std::vector<std::array<double, 50>> matrixE;
    std::string specfile;
    for (int i = 0; i < filenamelist.size(); i++)
    {
        std::cout << "processing file: " << filenamelist.at(i) << std::endl;
        // 1st step
        spectrum_fit(filenamelist.at(i));
        specfile = filenamelist.at(i).substr(0, filenamelist.at(i).find_last_of('.')).append("_spec.root");
        // 2nd step
        parameter_get(specfile, mean, error);
        std::copy(std::begin(mean), std::end(mean), arraytemp.begin());
        matrixM.push_back(arraytemp);
        std::copy(std::begin(error), std::end(error), arraytemp.begin());
        matrixE.push_back(arraytemp);
    }
    TGraphErrors *grHG[25], *grLG[25];
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            grHG[5 * i + j] = new TGraphErrors(chargelist.size());
            grLG[5 * i + j] = new TGraphErrors(chargelist.size());
            grHG[5 * i + j]->SetNameTitle(Form("Hit_%d_%d_HGCal", i + 1, j + 1), Form("Hit_%d_%d_HGCal;charge[fc];ADCvalue", i + 1, j + 1));
            grLG[5 * i + j]->SetNameTitle(Form("Hit_%d_%d_LGCal", i + 1, j + 1), Form("Hit_%d_%d_LGCal;charge[fc];ADCvalue", i + 1, j + 1));
            grHG[5 * i + j]->SetMarkerStyle(8);
            grHG[5 * i + j]->SetMarkerSize(0.7);
            grLG[5 * i + j]->SetMarkerStyle(8);
            grLG[5 * i + j]->SetMarkerSize(0.7);
        }
    for (int i = 0; i < chargelist.size(); i++)
        for (int j = 0; j < 50; j++)
        {
            if (j < 25)
            {
                int channelID = j;
                grLG[channelID]->SetPoint(i, chargelist.at(i), matrixM.at(i)[j]);
                grLG[channelID]->SetPointError(i, 0, matrixE.at(i)[j]);
                grLG[channelID]->SetPointError(i, 0, 0);
            }
            else
            {
                int channelID = j - 25;
                grHG[channelID]->SetPoint(i, chargelist.at(i), matrixM.at(i)[j]);
                grHG[channelID]->SetPointError(i, 0, matrixE.at(i)[j]);
                grHG[channelID]->SetPointError(i, 0, 0);
            }
        }

    TCanvas *canLG = new TCanvas("canLG", "canLG", 1600, 900);
    TCanvas *canHG = new TCanvas("canHG", "canHG", 1600, 900);
    TPaveStats *pave;
    canLG->Divide(5, 5);
    canHG->Divide(5, 5);
    for (int i = 0; i < 25; i++)
    {
        canLG->cd(ch2can(i));
        grLG[i]->Fit("pol1", "QR", "", grLG[i]->GetXaxis()->GetXmin(), grLG[i]->GetXaxis()->GetXmax());
        grLG[i]->Draw("ap");
        gPad->Modified();
        gPad->Update();
        pave = (TPaveStats *)grLG[i]->FindObject("stats");
        pave->SetX1NDC(0.1);
        pave->SetX2NDC(0.5);
        pave->SetY1NDC(0.5);
        pave->SetY2NDC(0.9);
        pave->SetFillStyle(0);
        pave->SetTextColor(kRed);
        gPad->Modified();
        gPad->Update();

        canHG->cd(ch2can(i));
        // grHG[i]->Fit("pol1", "QR", "", grHG[i]->GetXaxis()->GetXmin(), grHG[i]->GetXaxis()->GetXmax());
        // 2024
        // grHG[i]->Fit("pol1", "QR", "", 0, 500);
        // 2025
        grHG[i]->Fit("pol1", "QR", "", 0, 1000);
        grHG[i]->Draw("ap");
        grHG[i]->GetYaxis()->SetRangeUser(0,18000);
        grHG[i]->GetXaxis()->SetRangeUser(0, 1700);
        gPad->Modified();
        gPad->Update();
        pave = (TPaveStats *)grHG[i]->FindObject("stats");
        pave->SetX1NDC(0.1);
        pave->SetX2NDC(0.5);
        pave->SetY1NDC(0.5);
        pave->SetY2NDC(0.9);
        pave->SetFillStyle(0);
        pave->SetTextColor(kRed);
        gPad->Modified();
        gPad->Update();
    }
    TFile *outfile = new TFile("calibration.root", "RECREATE");
    outfile->cd();
    canLG->Write();
    canHG->Write();
    for (int i = 0; i < 25; i++)
    {
        grLG[i]->Write();
        grHG[i]->Write();
    }
    outfile->Close();
    double elecCal[50];
    calipars_get("calibration.root", elecCal);
    std::cout << "LG electric factor: " << std::endl;
    for (int i = 0; i < 25; i++)
        std::cout << elecCal[i] << std::endl;
    std::cout << "HG electric factor: " << std::endl;
    for (int i = 25; i < 50; i++)
        std::cout << elecCal[i] << std::endl;
    return 0;
}