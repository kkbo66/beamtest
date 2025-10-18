#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "TString.h"
#include "data_model.hh"
#include "RecHit.hh"
#include "Hit2Cluster.hh"
#include "Cluster.hh"
#include "Shower.hh"
#include "Neighbor.hh"
#include "Parameter.hh"
#include "SeedFinder.hh"
#include "Cluster2Shower.hh"
#include <TFile.h>
#include <TProfile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <map>
#include <vector>
#include "TChain.h"
#include <eigen3/Eigen/Dense>
#include <assert.h>

using namespace std;
using namespace TMath;
using namespace Eigen;

Double_t ff(double *x, double *par)
{
  Double_t val = par[0] * exp(-(x[0] - par[1]) / par[2]) + par[3] * exp(-(x[0] - par[1]) / par[4]) + par[5] * exp(-(x[0] - par[1]) / par[6]) + par[7] * exp(-(x[0] - par[1]) / par[8]) + par[9] * exp(-(x[0] - par[1]) / par[10]);

  if (x[0] >= par[1] && x[0] <= 2000)
    return val * Power((x[0] - par[1]), 2);
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

  Long64_t EventID;
  int TriggerID;
  Long64_t TimeCode;
  tree->SetBranchAddress("EventID", &EventID);
  tree->SetBranchAddress("TriggerID", &TriggerID);
  tree->SetBranchAddress("TimeCode", &TimeCode);
  vector<decode_data_col *> Hit(25);
  string Name[25];
  for (int i = 0; i < 25; i++)
  {
    int no;
    if (i % 5 == 0)
      no = 3;
    if (i % 5 == 1)
      no = 4;
    if (i % 5 == 2)
      no = 5;
    if (i % 5 == 3)
      no = 1;
    if (i % 5 == 4)
      no = 2;
    Name[i] = to_string(i / 5 + 1) + "_" + to_string(no);
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
    outputfile = "Reconstruction.root";
    cout << "Auto save file as Reconstruction.root..." << endl;
  }

  TFile *f = new TFile(outputfile.Data(), "recreate");

  bool IsRecTime = false;
  if (argc == 4)
    IsRecTime = true;

  Long64_t eventID;
  int triggerID;
  Long64_t timecode;
  vector<int> HitID;
  vector<double> HitEnergy;
  vector<double> HitTime;
  vector<int> ClusterID;
  vector<int> ClusterSize;
  vector<double> ClusterEnergy;
  vector<double> ClusterSMoment;
  vector<double> ClusterPosX;
  vector<double> ClusterPosY;
  vector<double> ClusterPosZ;
  vector<int> ShowerID;
  vector<double> ShowerE3x3;
  vector<double> ShowerE5x5;
  vector<double> ShowerEAll;
  vector<double> ShowerPosX3x3;
  vector<double> ShowerPosY3x3;
  vector<double> ShowerPosZ3x3;
  vector<double> ShowerPosX5x5;
  vector<double> ShowerPosY5x5;
  vector<double> ShowerPosZ5x5;
  vector<double> ShowerSMoment;
  vector<double> ShowerLatMoment;
  vector<double> ShowerA20Moment;
  vector<double> ShowerA42Moment;

  TTree *rec_data = new TTree("rec_data", "rec_data");
  rec_data->Branch("eventid", &eventID, "eventID/L");
  rec_data->Branch("triggerID", &triggerID, "triggerID/I");
  rec_data->Branch("timecode", &timecode, "timecode/L");
  rec_data->Branch("HitID", &HitID);
  rec_data->Branch("HitEnergy", &HitEnergy);
  rec_data->Branch("HitTime", &HitTime);
  rec_data->Branch("ClusterID", &ClusterID);
  rec_data->Branch("ClusterSize", &ClusterSize);
  rec_data->Branch("ClusterEnergy", &ClusterEnergy);
  rec_data->Branch("ClusterSMoment", &ClusterSMoment);
  rec_data->Branch("ClusterPosX", &ClusterPosX);
  rec_data->Branch("ClusterPosY", &ClusterPosY);
  rec_data->Branch("ClusterPosZ", &ClusterPosZ);
  rec_data->Branch("ShowerID", &ShowerID);
  rec_data->Branch("ShowerE3x3", &ShowerE3x3);
  rec_data->Branch("ShowerE5x5", &ShowerE5x5);
  rec_data->Branch("ShowerEAll", &ShowerEAll);
  rec_data->Branch("ShowerPosX3x3", &ShowerPosX3x3);
  rec_data->Branch("ShowerPosY3x3", &ShowerPosY3x3);
  rec_data->Branch("ShowerPosZ3x3", &ShowerPosZ3x3);
  rec_data->Branch("ShowerPosX5x5", &ShowerPosX5x5);
  rec_data->Branch("ShowerPosY5x5", &ShowerPosY5x5);
  rec_data->Branch("ShowerPosZ5x5", &ShowerPosZ5x5);
  rec_data->Branch("ShowerSMoment", &ShowerSMoment);
  rec_data->Branch("ShowerLatMoment", &ShowerLatMoment);
  rec_data->Branch("ShowerA20Moment", &ShowerA20Moment);
  rec_data->Branch("ShowerA42Moment", &ShowerA42Moment);

  Parameter &Para = Parameter::GetInstance();

  TF1 *f1 = new TF1("f", ff, 0, 2000, 11);

  TRandom3 r1;
  r1.SetSeed(time(0));

  int nEntries = tree->GetEntries();
  int interval = nEntries / 20;
  for (int i = 0; i < nEntries; i++)
  {
    // for(int i=500;i<nEntries;i++){
    int progress = static_cast<float>(i + 1) / nEntries * 100;
    if ((i + 1) % interval == 0)
    {
      cout << "Progress: " << progress + 1 << "%\r" << endl;
      std::cout.flush();
    }

    tree->GetEntry(i);

    eventID = EventID;
    triggerID = TriggerID;
    timecode = TimeCode;
    // timecode=i;
    // cout<<"time:"<<timecode<<endl;

    HitID.clear();
    HitEnergy.clear();
    HitTime.clear();
    ClusterID.clear();
    ClusterSize.clear();
    ClusterEnergy.clear();
    ClusterSMoment.clear();
    ClusterPosX.clear();
    ClusterPosY.clear();
    ClusterPosZ.clear();
    ShowerID.clear();
    ShowerE3x3.clear();
    ShowerE5x5.clear();
    ShowerEAll.clear();
    ShowerPosX3x3.clear();
    ShowerPosY3x3.clear();
    ShowerPosZ3x3.clear();
    ShowerPosX5x5.clear();
    ShowerPosY5x5.clear();
    ShowerPosZ5x5.clear();
    ShowerSMoment.clear();
    ShowerLatMoment.clear();
    ShowerA20Moment.clear();
    ShowerA42Moment.clear();

    vector<int> SeedIDVec;
    map<int, RecHit> HitMap;
    map<int, Cluster> ClusterMap;
    map<int, Shower> ShowerMap;

    map<int, RecHit>::iterator Hiter;
    map<int, Cluster>::iterator Citer;
    map<int, Shower>::iterator Siter;

    SeedIDVec.clear();
    HitMap.clear();
    ClusterMap.clear();
    ShowerMap.clear();

    for (int i = 0; i < 25; i++)
    {
      RecHit rechit;
      rechit.Clear();
      rechit.setCrystalID(Hit[i]->CrystalID);
      rechit.setFrontCenter(TVector3(Para.HitPosX(Hit[i]->CrystalID), Para.HitPosY(Hit[i]->CrystalID), Para.HitPosZ(Hit[i]->CrystalID)));
      rechit.setEnergy(Hit[i]->HighGainPeak, Hit[i]->LowGainPeak, Para.Ratio(i), Para.HGSatuPoint(i), Para.HGPedestal(i), Para.HGNoise(i), Para.LGPedestal(i), Para.LGNoise(i), Para.LGMipPeak(i), r1);

      if (IsRecTime)
      {
        // if(Hit[i]->HighGainPeak>4000){
        if (Hit[i]->HighGainPeak > 14000)
        {
          double HGA[256], LGA[256], T[256];
          for (int j = 0; j < 256; j++)
          {
            HGA[j] = Hit[i]->HAmplitude[j];
            LGA[j] = Hit[i]->LAmplitude[j];
            T[j] = j * 12.5;
          }

          int MaxID = 0;
          // double MaxAmp=0;

          int HGMaxID = 0;
          double HGMaxAmp = 0;

          int LGMaxID = 0;
          double LGMaxAmp = 0;

          for (int j = 40; j < 80; j++)
          {
            if (HGA[j] > HGMaxAmp)
            {
              HGMaxAmp = HGA[j];
              HGMaxID = j;
            }
          }

          for (int j = 40; j < 80; j++)
          {
            if (LGA[j] > LGMaxAmp)
            {
              LGMaxAmp = LGA[j];
              LGMaxID = j;
            }
          }

          if (Hit[i]->HighGainPeak < (Para.HGSatuPoint(i) - 400))
            MaxID = HGMaxID;
          else
            MaxID = LGMaxID;

          int cc = 40;
          int dd = 10;

          VectorXd hgtimingvec(cc);
          VectorXd lgtimingvec(cc);

          for (int j = 0; j < cc; j++)
          {
            hgtimingvec(j) = HGA[MaxID - dd + j];
            lgtimingvec(j) = LGA[MaxID - dd + j];
          }

          double time, amp, pedestal, chi2;

          if (Hit[i]->HighGainPeak < (Para.HGSatuPoint(i) - 400))
          {
            for (int j = 0; j < 11; j++)
              f1->SetParameter(j, Para.HGWfPara(i, j));
            f1->SetParameter(1, 0);
            OnePulseFit(cc, hgtimingvec, &time, &amp, &pedestal, &chi2, f1);
          }
          else
          {
            for (int j = 0; j < 11; j++)
              f1->SetParameter(j, Para.LGWfPara(i, j));
            f1->SetParameter(1, 0);
            OnePulseFit(cc, lgtimingvec, &time, &amp, &pedestal, &chi2, f1);
          }

          rechit.setTime((MaxID) * 12.5 - dd * 12.5 + time);

          HitID.push_back(rechit.CrystalID());
          HitTime.push_back(rechit.Time());
          HitEnergy.push_back(rechit.Energy());
        }
      }

      if (rechit.Energy() > 0)
        HitMap[Hit[i]->CrystalID] = rechit;

      if (rechit.Energy() > 0)
      {
        HitID.push_back(rechit.CrystalID());
        HitTime.push_back(rechit.Time());
        HitEnergy.push_back(rechit.Energy());
      }
    }

    Hit2Cluster m_Hit2Cluster;
    m_Hit2Cluster.Convert(HitMap, ClusterMap);
    Cluster2Shower m_Cluster2Shower;
    m_Cluster2Shower.Convert(ClusterMap, ShowerMap);

    /*SeedFinder m_SeedFinder;
      Citer=ClusterMap.begin();
      m_SeedFinder.Seed(Citer->second,SeedIDVec);
      for(int i=0;i<SeedIDVec.size();i++) cout<<"Seed:"<<SeedIDVec.at(i)<<endl;*/

    double epsilon = 1e-10;
    for (Citer = ClusterMap.begin(); Citer != ClusterMap.end(); Citer++)
    {
      // Calculate cluster position
      TVector3 possum;
      double etot = 0;
      for (auto it = Citer->second.Begin(); it != Citer->second.End(); ++it)
      {
        etot += it->second.Energy();
        TVector3 pos(it->second.FrontCenter());
        possum += pos * it->second.Energy();
      }
      if (etot > 0)
        possum = possum * (1 / etot);
      if (std::abs(possum.x() - (std::floor(possum.x() * 100000) / 100000)) < epsilon)
        Citer->second.setPosition(TVector3(std::floor(possum.x() * 100000) / 100000, std::floor(possum.y() * 100000) / 100000, std::floor(possum.z() * 100000) / 100000));
      else
        Citer->second.setPosition(possum);

      // Calculate cluster second moment
      double sum = 0;
      for (auto it = Citer->second.Begin(); it != Citer->second.End(); ++it)
      {
        TVector3 pos(it->second.FrontCenter());
        if (std::abs((pos - possum).Mag2()) < epsilon)
          sum += it->second.Energy() * 0;

        else
          sum += it->second.Energy() * ((pos - possum).Mag2());
      }
      if (etot > 0)
        sum /= etot;
      Citer->second.setSecondMoment(sum);
    }

    for (Citer = ClusterMap.begin(); Citer != ClusterMap.end(); Citer++)
    {
      ClusterID.push_back(Citer->second.GetClusterID());
      ClusterSize.push_back(Citer->second.GetClusterSize());
      ClusterEnergy.push_back(Citer->second.GetClusterEnergy());
      ClusterSMoment.push_back(Citer->second.GetSecondMoment());
      ClusterPosX.push_back(Citer->second.GetPosition().x());
      ClusterPosY.push_back(Citer->second.GetPosition().y());
      ClusterPosZ.push_back(Citer->second.GetPosition().z());
    }

    for (Siter = ShowerMap.begin(); Siter != ShowerMap.end(); Siter++)
    {
      ShowerID.push_back(Siter->second.SeedID());
      ShowerE3x3.push_back(Siter->second.E3x3());
      ShowerE5x5.push_back(Siter->second.E5x5());
      ShowerEAll.push_back(Siter->second.EAll());
      ShowerPosX3x3.push_back(Siter->second.x3x3());
      ShowerPosY3x3.push_back(Siter->second.y3x3());
      ShowerPosZ3x3.push_back(Siter->second.z3x3());
      ShowerPosX5x5.push_back(Siter->second.x5x5());
      ShowerPosY5x5.push_back(Siter->second.y5x5());
      ShowerPosZ5x5.push_back(Siter->second.z5x5());
      ShowerSMoment.push_back(Siter->second.SecondMoment());
      ShowerLatMoment.push_back(Siter->second.LateralMoment());
      ShowerA20Moment.push_back(Siter->second.A20Moment());
      ShowerA42Moment.push_back(Siter->second.A42Moment());
    }

    rec_data->Fill();
  }

  f->cd();
  rec_data->Write();
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
