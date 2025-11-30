#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
using namespace std;
using namespace TMath;

Double_t ff(double *x, double *par)
{
    Double_t val = par[0] * exp(-(x[0] - par[1]) / par[2]) + par[3] * exp(-(x[0] - par[1]) / par[4]) + par[5] * exp(-(x[0] - par[1]) / par[6]) + par[7] * exp(-(x[0] - par[1]) / par[8]);

    if (x[0] >= par[1] && x[0] <= 3000)
        return val * Power((x[0] - par[1]), 2);
    else
        return 0;
}

void seek_parameters(vector<TString> rootfiles)
{
    gStyle->SetOptFit(1111);
    double threshold_down = 2000, threshold_up = 13000;
    TChain *chain = new TChain("decode_data");
    for (auto &file : rootfiles)
    {
        chain->Add(file);
        cout << "Added file: " << file << endl;
    }
    int nEntries = chain->GetEntries();
    cout << "Total entries in chain: " << nEntries << endl;
    double HGamp[25][256], LGamp[25][256];
    int chNevent[25];
    memset(HGamp, 0, sizeof(HGamp));
    memset(LGamp, 0, sizeof(LGamp));
    // 每个通道所有过阈事例的波形取平均
    for (Int_t i = 0; i < 5; i++)
    {
        for (Int_t j = 0; j < 5; j++)
        {
            int ch = 5 * i + j;
            cout << "Register Channel " << i + 1 << "_" << j + 1 << " ... " << flush << endl;
            TTreeReader myReader(chain);
            TTreeReaderValue<Double_t> LowGainPedestal(myReader, Form("Hit_%d_%d.LowGainPedestal", i + 1, j + 1));
            TTreeReaderValue<Double_t> HighGainPedestal(myReader, Form("Hit_%d_%d.HighGainPedestal", i + 1, j + 1));
            TTreeReaderValue<Double_t> LowGainPeak(myReader, Form("Hit_%d_%d.LowGainPeak", i + 1, j + 1));
            TTreeReaderValue<Double_t> HighGainPeak(myReader, Form("Hit_%d_%d.HighGainPeak", i + 1, j + 1));
            TTreeReaderArray<Double_t> LAmplitude(myReader, Form("Hit_%d_%d.LAmplitude", i + 1, j + 1));
            TTreeReaderArray<Double_t> HAmplitude(myReader, Form("Hit_%d_%d.HAmplitude", i + 1, j + 1));
            while (myReader.Next())
            {
                double HGMIP = *HighGainPeak - *HighGainPedestal;
                if (HGMIP > threshold_down && HGMIP < threshold_up)
                {
                    chNevent[ch]++;
                }
                else
                    continue;
                if (LAmplitude.GetSize() != 256 || HAmplitude.GetSize() != 256)
                {
                    std::cerr << "amplitude points is not 256!" << std::endl;
                    continue;
                }

                for (size_t k = 0; k < LAmplitude.GetSize(); k++)
                {
                    HGamp[ch][k] += (HAmplitude[k] - *HighGainPedestal);
                    LGamp[ch][k] += (LAmplitude[k] - *LowGainPedestal);
                }
            }
        }
    }
    int HGpeakpos[25], LGpeakpos[25];
    double HGmax, LGmax;
    // 波形取平均，并寻找峰值位置
    for (int i = 0; i < 25; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            HGamp[i][j] /= chNevent[i];
            LGamp[i][j] /= chNevent[i];
        }
        HGmax = 0, LGmax = 0;
        for (int j = 40; j < 120; j++)
        {
            if (HGamp[i][j] > HGmax)
            {
                HGmax = HGamp[i][j];
                HGpeakpos[i] = j;
            }
            if (LGamp[i][j] > LGmax)
            {
                LGmax = LGamp[i][j];
                LGpeakpos[i] = j;
            }
        }
    }
    // 拟合函数为5个指数乘上幂函数的形式，拟合所有高低增益通道
    double HGpars[25][9], LGpars[25][9];
    double HGInitpars[9] = {2.40185, 520.663, 49.1514, -4.54368, 44.1523, 7.10989, 53.7494, -4.97743, 49.5134};
    double LGInitpars[9] = {2.26025, 521.106, 54.1251, -0.639271, 39.6453, 2.2916, 50.2358, -3.91322, 52.6975};
    TF1 *fun_HG = new TF1("funHG", ff, 0, 3000, 9);
    TF1 *fun_LG = new TF1("funLG", ff, 0, 3000, 9);
    TGraph *gr_HG = new TGraph(256);
    gr_HG->SetMarkerStyle(8);
    gr_HG->SetMarkerSize(0.5);
    TGraph *gr_LG = new TGraph(256);
    gr_LG->SetMarkerStyle(8);
    gr_LG->SetMarkerSize(0.5);
    TCanvas *can_HG = new TCanvas("canHG", "canHG", 1600, 900);
    can_HG->Divide(5, 5);
    TCanvas *can_LG = new TCanvas("canLG", "canLG", 1600, 900);
    can_LG->Divide(5, 5);
    TFile *outputfile = new TFile("average_waveform.root", "RECREATE");
    for (int i = 0; i < 25; i++)
    {
        fun_HG->SetParameters(HGInitpars);
        fun_LG->SetParameters(LGInitpars);
        for (int j = 0; j < 256; j++)
        {
            gr_HG->SetPoint(j, 12.5 * j, HGamp[i][j]);
            gr_LG->SetPoint(j, 12.5 * j, LGamp[i][j]);
        }
        gr_HG->SetName(Form("HGch%d", i));
        gr_LG->SetName(Form("LGch%d", i));
        gr_HG->Write();
        gr_LG->Write();
        for (int j = 0; j < 10; j++)
        {
            gr_HG->Fit(fun_HG, "R", "", (HGpeakpos[i] - 12) * 12.5, (HGpeakpos[i] + 30) * 12.5);
            gr_LG->Fit(fun_LG, "R", "", (LGpeakpos[i] - 12) * 12.5, (LGpeakpos[i] + 30) * 12.5);
            fun_HG->GetParameters(HGInitpars);
            fun_LG->GetParameters(LGInitpars);
        }
        can_HG->cd(i / 5 + (4 - i % 5) * 5 + 1);
        gr_HG->DrawClone("ap");
        can_LG->cd(i / 5 + (4 - i % 5) * 5 + 1);
        gr_LG->DrawClone("ap");
        for (int j = 0; j < 9; j++)
        {
            HGpars[i][j] = fun_HG->GetParameter(j);
            LGpars[i][j] = fun_LG->GetParameter(j);
        }
    }
    outputfile->Close();
    can_HG->SaveAs("HG_fun_fit.png");
    can_LG->SaveAs("LG_fun_fit.png");
    // 将拟合参数保存到参数文件
    std::ofstream outfile("WaveformPara.dat");
    if (!outfile.is_open())
    {
        std::cerr << "open file error!" << std::endl;
    }
    for (int i = 0; i < 25; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            outfile << left << setw(15) << LGpars[i][j] << '\t';
        }
        outfile << '\n';
        for (int j = 0; j < 9; j++)
        {
            outfile << left << setw(15) << HGpars[i][j] << '\t';
        }
        outfile << '\n';
    }
    outfile.close();
}
// 输入root文件或者文件列表
int main(int argv, char *argc[])
{
    if (argv < 2)
    {
        cout << "Please input root file or (rootlist).txt" << endl;
        return 1;
    }
    vector<TString> rootfiles;
    rootfiles.clear();
    if (string(argc[1]).find(".txt") != string::npos)
    {
        ifstream infile;
        infile.open(argc[1], ios::in);
        string line;
        while (getline(infile, line))
        {
            TString rootname = line;
            rootfiles.push_back(rootname);
        }
        infile.close();
    }
    else if (string(argc[1]).find(".root") != string::npos)
    {
        TString rootname = argc[1];
        rootfiles.push_back(rootname);
    }
    else
    {
        cout << "Please input root file or (rootlist).txt" << endl;
        return 1;
    }
    seek_parameters(rootfiles);
    return 0;
}
