#include <string>
#include <iostream>
#include <fstream>
#include "TString.h"
#include "data_model.hh"
#include <TFile.h>
#include <TProfile.h>
#include <TTree.h>
#include <TH1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TF1.h>
#include "TChain.h"

#include "Parameter.hh"
#include "Decode2025.hh"
#include <eigen3/Eigen/Dense>
#include <assert.h>
using namespace std;
using namespace TMath;
using namespace Eigen;
// 1024run10调延迟后，滞后32个点

Double_t ff(double *x, double *par)
{
    Double_t val = par[0] * exp(-(x[0] - par[1]) / par[2]) + par[3] * exp(-(x[0] - par[1]) / par[4]) + par[5] * exp(-(x[0] - par[1]) / par[6]) + par[7] * exp(-(x[0] - par[1]) / par[8]);

    if (x[0] >= par[1] && x[0] <= 3000)
        return val * TMath::Power((x[0] - par[1]), 2);
    else
        return 0;
}

bool isTextFile(const string &fileName)
{
  size_t dotPos = fileName.find_last_of('.');
  if (dotPos == std::string::npos)
    return false;

  std::string extension = fileName.substr(dotPos + 1);
  if (extension == "txt")
    return true;
  else
    return false;
}

bool OnePulseFit(int cc, VectorXd timingvec, double *time, double *amp, double *pedestal, double *chi2, TF1 *f1);

int main(int argc, char const *argv[])
{
  vector<string> datafiles;

  if (isTextFile(argv[1]))
  {
    ifstream filestream(argv[1]);
    if (filestream.is_open())
    {
      string filename;
      while (getline(filestream, filename))
        datafiles.push_back(filename);
    }
    else
      cout << "error in reading filelist  " << endl;
  }
  else
    datafiles.push_back(argv[1]);

  TChain *tree = new TChain("decode_data");
  for (size_t i = 0; i < datafiles.size(); i++)
    tree->AddFile(datafiles.at(i).data());
  int TriggerID;
  tree->SetBranchAddress("TriggerID", &TriggerID);
  vector<decode_data_col *> Hit(25);
  string Name[25];
  for (int i = 0; i < 25; i++)
  {
    Name[i] = to_string(i / 5 + 1) + "_" + to_string(i % 5 + 1);
  }
  for (int i = 0; i < 25; i++)
    Hit[i] = new decode_data_col();
  for (int i = 0; i < 25; i++)
  {
    string name = "Hit_" + Name[i];
    tree->SetBranchAddress(name.data(), Hit[i]);
  }

  TString outputfile;
  if (argc >= 3)
    outputfile = argv[2];
  else
  {
    outputfile = "ECALDigi.root";
    cout << "Auto save file as ECALDigi.root..." << endl;
  }
  // true：调整寻峰窗口延迟
  bool TimeDelay;
  if (argc >= 4)
  {
    if (std::stoi(argv[3]) == 1)
      TimeDelay = true;
    else
      TimeDelay = false;
  }

  TFile *f = new TFile(outputfile.Data(), "recreate");

  int triggerID;
  vector<int> CrystalID;
  vector<double> TimeHG;
  vector<double> TimeLG;
  vector<double> AmpHG_Peak; // ADC for High Gain from peak finding
  vector<double> AmpLG_Peak; // ADC for Low Gain from peak finding
  vector<double> AmpHG_Fit;  // ADC for High Gain from fitting
  vector<double> AmpLG_Fit;  // ADC for Low Gain from fitting

  TTree *ECALdigi_data = new TTree("ECALdigi_data", "ECALdigi_data");
  ECALdigi_data->Branch("EventID", &triggerID, "triggerID/I");
  ECALdigi_data->Branch("CrystalID", &CrystalID);
  ECALdigi_data->Branch("TimeLG", &TimeLG);
  ECALdigi_data->Branch("TimeHG", &TimeHG);
  ECALdigi_data->Branch("AmpHG_Peak", &AmpHG_Peak);
  ECALdigi_data->Branch("AmpLG_Peak", &AmpLG_Peak);
  ECALdigi_data->Branch("AmpHG_Fit", &AmpHG_Fit);
  ECALdigi_data->Branch("AmpLG_Fit", &AmpLG_Fit);

  Parameter &Para = Parameter::GetInstance();

  TF1 *f1 = new TF1("f", ff, 0, 3000, 9);

  int nEntries = tree->GetEntries();
  int interval = nEntries / 20;
  
  for (int i = 0; i < nEntries; i++)
  {
    int progress = static_cast<float>(i + 1) / nEntries * 100;
    if ((i + 1) % interval == 0)
    {
      cout << "Progress: " << progress + 1 << "%\r" << endl;
      std::cout.flush();
    }

    tree->GetEntry(i);

    triggerID = TriggerID;

    CrystalID.clear();
    TimeHG.clear();
    TimeLG.clear();
    AmpHG_Peak.clear();
    AmpLG_Peak.clear();
    AmpHG_Fit.clear();
    AmpLG_Fit.clear();

    for (int k = 0; k < 25; k++)
    {
      CrystalID.push_back(Hit[k]->CrystalID);
      AmpHG_Peak.push_back(Hit[k]->HighGainPeak);
      AmpLG_Peak.push_back(Hit[k]->LowGainPeak);

      // Fit
      double HGA[256], LGA[256], T[256];
      for (int j = 0; j < 256; j++)
      {
        HGA[j] = Hit[k]->HAmplitude[j];
        LGA[j] = Hit[k]->LAmplitude[j];
        T[j] = j * 12.5;
      }

      int HGMaxID = 0;
      double HGMaxAmp = 0;

      int LGMaxID = 0;
      double LGMaxAmp = 0;

      int Pstart = 140, Pstop = 170;
      // int Pstart = 65, Pstop = 95;
      if (TimeDelay)
      {
        Pstart += 35;
        Pstop += 35;
      }
      for (int j = Pstart; j < Pstop; j++)
      {
        if (HGA[j] >= HGMaxAmp)
        {
          HGMaxAmp = HGA[j];
          HGMaxID = j;
        }
      }

      for (int j = Pstart; j < Pstop; j++)
      {
        if (LGA[j] >= LGMaxAmp)
        {
          LGMaxAmp = LGA[j];
          LGMaxID = j;
        }
      }

      bool IsOsc = false;
      if ((HGMaxID < (Pstart + 10) || HGMaxID > (Pstop - 10)) && HGMaxAmp < 16000)
        IsOsc = true;

      int cc = 40;
      int dd = 10;

      VectorXd hgtimingvec(cc);
      VectorXd lgtimingvec(cc);

      for (int j = 0; j < cc; j++)
      {
        hgtimingvec(j) = HGA[HGMaxID - dd + j];
        lgtimingvec(j) = LGA[LGMaxID - dd + j];
      }

      double time, amp, pedestal, chi2;
      // if (triggerID == 0)
      // {
      //   for (int j = 0; j < 9; j++)
      //     f1->SetParameter(j, Para.HGWfPara(k, j));
      //   f1->Draw();
      //   gPad->SaveAs(Form("fitfunctionHG_%d.png", k));
      //   for (int j = 0; j < 9; j++)
      //     f1->SetParameter(j, Para.LGWfPara(k, j));
      //   f1->Draw();
      //   gPad->SaveAs(Form("fitfunctionLG_%d.png", k));
      // }
      if (Hit[k]->HighGainPeak > (Para.HGPedestal(k) + 6 * Para.HGNoise(k)) && Hit[k]->HighGainPeak < 16000 && IsOsc == false)
      {
        for (int j = 0; j < 9; j++)
          f1->SetParameter(j, Para.HGWfPara(k, j));
        f1->SetParameter(1, 0);
        OnePulseFit(cc, hgtimingvec, &time, &amp, &pedestal, &chi2, f1);
        TimeHG.push_back((HGMaxID) * 12.5 - dd * 12.5 + time);
        AmpHG_Fit.push_back(amp * f1->GetMaximum());
        // TimeHG.push_back(HGMaxID * 12.5);
        // AmpHG_Fit.push_back(Hit[k]->HighGainPeak - Para.HGPedestal(k));
      }
      else
      {
        if (IsOsc == false)
        {
          TimeHG.push_back(999999);
          AmpHG_Fit.push_back(Hit[k]->HighGainPeak - Para.HGPedestal(k));
        }
        else
        {
          TimeHG.push_back(888888);
          AmpHG_Fit.push_back(0);
          // AmpHG_Fit.push_back(Hit[k]->HighGainPeak - Para.HGPedestal(k));
        }
      }

      if (Hit[k]->LowGainPeak > (Para.LGPedestal(k) + 6 * Para.LGNoise(k)) && Hit[k]->LowGainPeak < 16000 && IsOsc == false)
      {
        for (int j = 0; j < 9; j++)
          f1->SetParameter(j, Para.LGWfPara(k, j));
        f1->SetParameter(1, 0);
        OnePulseFit(cc, lgtimingvec, &time, &amp, &pedestal, &chi2, f1);
        TimeLG.push_back((LGMaxID) * 12.5 - dd * 12.5 + time);
        AmpLG_Fit.push_back(amp * f1->GetMaximum());
        // TimeLG.push_back(LGMaxID * 12.5);
        // AmpLG_Fit.push_back(Hit[k]->LowGainPeak - Para.LGPedestal(k));
      }
      else
      {
        if (IsOsc == false)
        {
          TimeLG.push_back(999999);
          AmpLG_Fit.push_back(Hit[k]->LowGainPeak - Para.LGPedestal(k));
        }
        else
        {
          TimeLG.push_back(888888);
          AmpLG_Fit.push_back(0);
          // AmpLG_Fit.push_back(Hit[k]->LowGainPeak - Para.LGPedestal(k));
        }
      }
    }

    ECALdigi_data->Fill();
  }

  f->cd();
  ECALdigi_data->Write();
  f->Close();

  return 0;
}

bool OnePulseFit(int cc, VectorXd timingvec, double *time, double *amp, double *pedestal, double *ochi2, TF1 *f1)
{

  VectorXd vec_ones(cc);
  MatrixXd mat_noi = MatrixXd::Zero(cc, cc);

  for (int i = 0; i < cc; i++)
  {
    vec_ones(i) = 1;
    mat_noi(i, i) = 1;
  }

  double t_trg = 0.0;
  // double t_trg = -400;
  VectorXd x(3);
  double fit_a, chi2, pes;
  double deltachi2, tchi2;
  chi2 = 9999;

  for (int l = 0; l < 6; l++)
  {
    VectorXd est_data(cc);
    VectorXd d_est_data(cc);
    for (int i = 0; i < cc; i++)
    {
      // double i1 = (i * 25 + t_trg);
      double i1 = (i * 12.5 + t_trg);
      est_data(i) = f1->Eval(i1);
      d_est_data(i) = f1->Derivative(i1);
    }
    MatrixXd A(3, 3);
    A(0, 0) = est_data.transpose() * mat_noi * est_data;
    A(0, 1) = est_data.transpose() * mat_noi * d_est_data;
    A(0, 2) = est_data.transpose() * mat_noi * vec_ones;
    A(1, 0) = d_est_data.transpose() * mat_noi * est_data;
    A(1, 1) = d_est_data.transpose() * mat_noi * d_est_data;
    A(1, 2) = d_est_data.transpose() * mat_noi * vec_ones;
    A(2, 0) = vec_ones.transpose() * mat_noi * est_data;
    A(2, 1) = vec_ones.transpose() * mat_noi * d_est_data;
    A(2, 2) = vec_ones.transpose() * mat_noi * vec_ones;

    VectorXd b(3);
    b(0) = est_data.transpose() * mat_noi * timingvec;
    b(1) = d_est_data.transpose() * mat_noi * timingvec;
    b(2) = vec_ones.transpose() * mat_noi * timingvec;
    x = A.colPivHouseholderQr().solve(b);
    double dt;
    dt = x(1) / x(0);
    t_trg = t_trg + dt;
    fit_a = x(0);
    pes = x(2);
    tchi2 = (timingvec - x(0) * est_data - x(1) * d_est_data - x(2) * vec_ones)
                .transpose() *
            mat_noi *
            (timingvec - x(0) * est_data - x(1) * d_est_data - x(2) * vec_ones);
    deltachi2 = chi2 - tchi2;
    chi2 = tchi2;
  }

  *time = -t_trg;
  *amp = fit_a;
  *pedestal = pes;
  *ochi2 = chi2;

  return 1;
}
