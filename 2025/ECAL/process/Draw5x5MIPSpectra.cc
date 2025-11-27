#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TString.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TChain.h>
#include <TROOT.h>
#include <TStyle.h>

#include "DataModel2025.hh"

using namespace std;
using namespace TMath;

double langaufun(Double_t *x, Double_t *par)
{

    // Fit parameters:
    // par[0]=Width (scale) parameter of Landau density
    // par[1]=Most Probable (MP, location) parameter of Landau density
    // par[2]=Total area (integral -inf to inf, normalization constant)
    // par[3]=Width (sigma) of convoluted Gaussian function
    //
    // In the Landau distribution (represented by the CERNLIB approximation),
    // the maximum is located at x=-0.22278298 with the location parameter=0.
    // This shift is corrected within this function, so that the actual
    // maximum is identical to the MP parameter.

    // Numeric constants
    Double_t invsq2pi = 0.3989422804014; // (2 pi)^(-1/2)
    Double_t mpshift = -0.22278298;      // Landau maximum location

    // Control constants
    Double_t np = 100.0; // number of convolution steps
    Double_t sc = 5.0;   // convolution extends to +-sc Gaussian sigmas

    // Variables
    Double_t xx;
    Double_t mpc;
    Double_t fland;
    Double_t sum = 0.0;
    Double_t xlow, xupp;
    Double_t step;
    Double_t i;

    // MP shift correction
    mpc = par[1] - mpshift * par[0];

    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];

    step = (xupp - xlow) / np;

    // Convolution integral of Landau and Gaussian by sum
    for (i = 1.0; i <= np / 2; i++)
    {
        xx = xlow + (i - .5) * step;
        fland = TMath::Landau(xx, mpc, par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0], xx, par[3]);

        xx = xupp - (i - .5) * step;
        fland = TMath::Landau(xx, mpc, par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0], xx, par[3]);
    }

    return (par[2] * step * sum * invsq2pi / par[3]);
}

void langaufit(TH1F *his)
{
    // Once again, here are the Landau * Gaussian parameters:
    //   par[0]=Width (scale) parameter of Landau density
    //   par[1]=Most Probable (MP, location) parameter of Landau density
    //   par[2]=Total area (integral -inf to inf, normalization constant)
    //   par[3]=Width (sigma) of convoluted Gaussian function
    //
    // Variables for langaufit call:
    //   his             histogram to fit
    //   startvalues[4]  reasonable start values for the fit
    //   parlimitslo[4]  lower parameter limits
    //   parlimitshi[4]  upper parameter limits

    if (his->GetEntries() < 500)
        return;
    TFitResultPtr fit_res;
    int binMax, binLeft, binRight;
    double LeftthresholdValue, RightthresholdValue, LeftValue, RightValue, LeftleftValue, Rightrightvalue;
    // determine fitting range
    binMax = his->GetMaximumBin();
    LeftthresholdValue = his->GetBinContent(binMax) / 4;
    RightthresholdValue = his->GetBinContent(binMax) / 8;
    binLeft = binRight = binMax;
    do
    {
        LeftValue = his->GetBinContent(binLeft);
        RightValue = his->GetBinContent(binRight);
        LeftleftValue = his->GetBinContent(binLeft - 1);
        Rightrightvalue = his->GetBinContent(binRight - 1);
        if (LeftValue > LeftthresholdValue || LeftleftValue > LeftthresholdValue)
            binLeft--;
        if (RightValue > RightthresholdValue || Rightrightvalue > RightthresholdValue)
            binRight++;
        if (binLeft == 0 || binRight == his->GetNbinsX())
            break;
    } while (LeftValue > LeftthresholdValue || RightValue > RightthresholdValue || LeftleftValue > LeftthresholdValue || Rightrightvalue > RightthresholdValue);

    // start values, parameter lower limits, parameter upper limits
    double StdDev, MaxPos, Area, Sigma;
    StdDev = his->GetStdDev();
    MaxPos = his->GetMaximumBin() * his->GetBinWidth(0);
    Area = his->Integral("width");
    double startvalues[4] = {StdDev, MaxPos, Area, 1}, parlimitslo[4] = {StdDev * 0.001, MaxPos * 0.1, Area * 0.01, 0.01}, parlimitshi[4] = {StdDev, MaxPos * 10, Area * 100, 100};

    TString FunName = "lan_gaus_conv";
    TF1 *ffitold = (TF1 *)gROOT->GetListOfFunctions()->FindObject(FunName);
    if (ffitold)
        delete ffitold;

    TF1 *ffit = new TF1(FunName.Data(), langaufun, his->GetBinCenter(binMax - 50), his->GetBinCenter(binMax + 150), 4);
    ffit->SetParameters(startvalues);
    ffit->SetParNames("Width", "MP", "Area", "GSigma");

    for (int i = 0; i < 4; i++)
    {
        ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
    }

    fit_res = his->Fit(FunName, "QS RB"); // fit within specified range, use ParLimits
}

void Draw5x5MIPSpectra(TString rootname)
{
    gStyle->SetOptStat(0000);
    // gStyle->SetOptFit(1110);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadBottomMargin(0.1);
    gStyle->SetStatX(0.7);
    gStyle->SetStatY(0.3);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetTitleFont(42, "xyz");
    gStyle->SetLabelFont(42, "xyz");
    gStyle->SetLegendFont(42);
    gStyle->SetTextFont(42);
    gStyle->SetTitleSize(0.05, "xyz");
    gStyle->SetLabelSize(0.04, "xyz");
    gStyle->SetLegendTextSize(0.04);
    gStyle->SetTitleOffset(1.2, "y");
    gStyle->SetTitleOffset(0.8, "x");
    gStyle->SetPadGridX(true);
    gStyle->SetPadGridY(true);
    size_t dotpos = rootname.Last('.');
    size_t slashpos = rootname.Last('/');
    TString filename = rootname(slashpos + 1, dotpos - slashpos - 1);
    TFile *infile = new TFile(rootname, "READ");
    TTree *tr = (TTree *)infile->Get("decode_data");
    Int_t Nentries = (Int_t)tr->GetEntries();

    DataModel2025 hit[25];
    TH1F *HGMip_his[25];

    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            tr->SetBranchAddress(Form("Hit_%d_%d", i + 1, j + 1), &hit[5 * i + j]);
            HGMip_his[5 * i + j] = new TH1F(Form("MIPSpectrum_%d_%d", i + 1, j + 1), Form("his%d;ADC value;counts", 5 * i + j), 200, 4000, 14000);
            // HGMip_his[5 * i + j] = new TH1F(Form("MIPSpectrum_%d_%d", i + 1, j + 1), Form("his%d;ADC value;counts", 5 * i + j), 200, 500, 1500);
        }
    double energycut = 2000, peak, temp;
    int channel, chcount, process;
    for (int n = 0; n < Nentries; n++)
    {
        if (n % (Nentries / 100) == 0 && n != 0)
        {
            process = 100 * n / (Nentries);
            std::cout << "\rProcessing: >>>>>>>>>>>>>>> " << process << "% <<<<<<<<<<<<<<<";
            std::cout.flush();
        }
        tr->GetEntry(n);
        chcount = 0;
        channel = -1;
        temp = 0;
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
            {
                int ch = 5 * i + j;
                // temp = hit[ch].LowGainPeak - hit[ch].LowGainPedestal;
                temp = hit[ch].HighGainPeak - hit[ch].HighGainPedestal;
                if (temp > energycut)
                {
                    chcount++;
                    channel = ch;
                    // peak = hit[ch].HighGainPeak;
                    peak = temp;
                }
            }
        if (chcount != 1)
            continue;
        else
        {
            if (peak > HGMip_his[channel]->GetBinCenter(0) && peak < HGMip_his[channel]->GetBinCenter(HGMip_his[channel]->GetNbinsX()))
                HGMip_his[channel]->Fill(peak);
        }
    }
    std::cout << std::endl;

    double MPV[25], Peak[25];
    TCanvas *can = new TCanvas("cMipSpectra", "canvas", 1600, 900);
    can->Divide(5, 5);
    double par[4] = {1000, 800, 10000, 10};
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            can->cd(21 + i - 5 * j);
            HGMip_his[5 * i + j]->Draw();
            TF1 *ff = new TF1("lan_gaus_conv", langaufun, HGMip_his[5 * i + j]->GetMaximumBin() * HGMip_his[5 * i + j]->GetBinWidth(0) + HGMip_his[5 * i + j]->GetBinLowEdge(1) - 1000, HGMip_his[5 * i + j]->GetMaximumBin() * HGMip_his[5 * i + j]->GetBinWidth(0) + HGMip_his[5 * i + j]->GetBinLowEdge(1) + 4000, 4);
            par[1] = HGMip_his[5 * i + j]->GetMaximumBin() * HGMip_his[5 * i + j]->GetBinWidth(0) + HGMip_his[5 * i + j]->GetBinLowEdge(1);
            ff->SetParameters(par);
            HGMip_his[5 * i + j]->Fit(ff, "QR");
            // langaufit(HGMip_his[5 * i + j]);
            if (HGMip_his[5 * i + j]->GetFunction("lan_gaus_conv"))
                MPV[5 * i + j] = HGMip_his[5 * i + j]->GetFunction("lan_gaus_conv")->GetMaximumX();
            Peak[5 * i + j] = HGMip_his[5 * i + j]->GetMaximumBin() * HGMip_his[5 * i + j]->GetBinWidth(0) + HGMip_his[5 * i + j]->GetBinLowEdge(1);
            HGMip_his[5 * i + j]->GetXaxis()->SetRangeUser(Peak[5 * i + j] - 2 * HGMip_his[5 * i + j]->GetRMS(), Peak[5 * i + j] + 5 * HGMip_his[5 * i + j]->GetRMS());
            HGMip_his[5 * i + j]->GetYaxis()->SetRangeUser(0, HGMip_his[5 * i + j]->GetMaximum() * 1.2);
        }
    TFile *fout = new TFile("./5x5MIP-" + filename + ".root", "RECREATE");
    fout->cd();
    can->Write();
    for (int i = 0; i < 25; i++)
    {
        HGMip_his[i]->Write();
    }
    fout->Close();
    std::ofstream outfile("MIPpeak.txt");
    outfile << "MIP peak value:\n";
    for (int i = 0; i < 25; i++)
        outfile << Peak[i] << ", ";
    outfile << "\nMIP MPV value:\n";
    for (int i = 0; i < 25; i++)
        outfile << MPV[i] << ", ";
    outfile.close();
    std::cout << "generate root file: " << "./5x5MIP-" + filename + ".root" << ", and parameter file: MIPpeak.txt" << std::endl;
}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv, char *argc[])
{
    if (argv < 2)
    {
        cout << "Usage: ./Draw5x5MIPSpectra decode file" << endl;
        return 1;
    }
    TString rootname = argc[1];
    Draw5x5MIPSpectra(rootname);
    return 0;
}
#endif