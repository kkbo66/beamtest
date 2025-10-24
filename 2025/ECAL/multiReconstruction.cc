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

using namespace std;

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
  double TimeStamp[25];
  vector<vector<double> *> CoarseTime;
  vector<vector<double> *> FineTime;
  vector<vector<double> *> Amplitude;
  for(int i=0;i<25;i++)
  {
    CoarseTime.push_back(new vector<double>);
    FineTime.push_back(new vector<double>);
    Amplitude.push_back(new vector<double>);
  }
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
    string timeName = name + "_TimeStamp";
    string coarseName = name + "_CoarseTime";
    string fineName = name + "_FineTime";
    string ampName = name + "_Amplitude";
    tree->SetBranchAddress(name.data(), Hit[i]);
    tree->SetBranchAddress(timeName.data(), &TimeStamp[i]);
    tree->SetBranchAddress(coarseName.data(), &CoarseTime[i]);
    tree->SetBranchAddress(fineName.data(), &FineTime[i]);
    tree->SetBranchAddress(ampName.data(), &Amplitude[i]);
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

  int recmode = 1;
  if (argc == 4)
    recmode = atoi(argv[3]);

  int triggerID;
  vector<int> HitID;
  vector<int> MatchShowerID;
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
  rec_data->Branch("EventID", &triggerID, "triggerID/I");
  rec_data->Branch("HitID", &HitID);
  rec_data->Branch("MatchShowerID", &MatchShowerID);
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

  TRandom3 r1;
  r1.SetSeed(time(0));

  const int npoint = 12;
  double hitenergy[npoint] = {0.005, 0.008, 0.01, 0.012, 0.014, 0.016, 0.02, 0.024, 0.032, 0.085, 0.09, 0.1};
  double delta_time[npoint] = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 10, 10};
  TGraph *timecut = new TGraph(npoint, hitenergy, delta_time);

  int nEntries = tree->GetEntries();
  int interval = nEntries / 20;
  for (int k = 0; k < nEntries; k++)
  {
    int progress = static_cast<float>(k + 1) / nEntries * 100;
    if ((k + 1) % interval == 0)
    {
      cout << "Progress: " << progress + 1 << "%\r" << endl;
      std::cout.flush();
    }

    tree->GetEntry(k);

    triggerID = TriggerID;

    HitID.clear();
    MatchShowerID.clear();
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

    if( recmode == 0 )
    {
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
        int CrystalID = Hit[i]->CrystalID;
        double timestamp = TimeStamp[i];
        int index = -1;
        double maxAmp = -1;
        for (int j = 0; j < Amplitude[i]->size(); j++)
        {
          double coarseTime = CoarseTime[i]->at(j);
          double fineTime = FineTime[i]->at(j);
          double amp = Amplitude[i]->at(j);
          double time = coarseTime - timestamp;
          if (amp > maxAmp)
          {
            maxAmp = amp;
            index = j;
          }
        }
        if (index != -1)
        {
          RecHit rechit;
          rechit.Clear();
          rechit.setCrystalID(CrystalID);
          rechit.setFrontCenter(TVector3(Para.HitPosX(CrystalID), Para.HitPosY(CrystalID), Para.HitPosZ(CrystalID)));
          double coarseTime = CoarseTime[i]->at(index);
          double fineTime = FineTime[i]->at(index);
          double amp = Amplitude[i]->at(index);
          double time = coarseTime - timestamp;
          rechit.setEnergy(amp, Para.HGNoise(i), Para.HGPedestal(i), Para.HGMipPeak(i));
          rechit.setTime(time);
          if (rechit.Energy() > 0)
          {
            HitMap[CrystalID] = rechit;
            HitID.push_back(rechit.CrystalID());
            HitTime.push_back(rechit.Time());
            HitEnergy.push_back(rechit.Energy());
          }
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
    }

    if( recmode == 1 )
    {
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
        int CrystalID = Hit[i]->CrystalID;
        double timestamp = TimeStamp[i];
        int index = -1;
        double maxAmp = -1;
        for (int j = 0; j < Amplitude[i]->size(); j++)
        {
          double coarseTime = CoarseTime[i]->at(j);
          double fineTime = FineTime[i]->at(j);
          double amp = Amplitude[i]->at(j);
          double time = coarseTime - timestamp;
          if (time > 400 && time < 500 && amp > maxAmp)
          {
            maxAmp = amp;
            index = j;
          }
        }
        if (index != -1)
        {
          RecHit rechit;
          rechit.Clear();
          rechit.setCrystalID(CrystalID);
          rechit.setFrontCenter(TVector3(Para.HitPosX(CrystalID), Para.HitPosY(CrystalID), Para.HitPosZ(CrystalID)));
          double coarseTime = CoarseTime[i]->at(index);
          double fineTime = FineTime[i]->at(index);
          double amp = Amplitude[i]->at(index);
          double time = coarseTime - timestamp;
          rechit.setEnergy(amp, Para.HGNoise(i), Para.HGPedestal(i), Para.HGMipPeak(i));
          rechit.setTime(time);
          if (rechit.Energy() > 0)
          {
            HitMap[CrystalID] = rechit;
            HitID.push_back(rechit.CrystalID());
            HitTime.push_back(rechit.Time());
            HitEnergy.push_back(rechit.Energy());
          }
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
    }

    if (recmode == 2)
    {
      vector<int> SeedIDVec;
      map<int, vector<RecHit>> HitMap;
      //map<int, RecHit> HitMap;
      multimap<int, Cluster> ClusterMap;
      multimap<int, Shower> ShowerMap;

      map<int, vector<RecHit>>::iterator Hiter;
      //map<int, RecHit>::iterator Hiter;
      multimap<int, Cluster>::iterator Citer;
      multimap<int, Shower>::iterator Siter;

      SeedIDVec.clear();
      HitMap.clear();
      ClusterMap.clear();
      ShowerMap.clear();

      for (int i = 0; i < 25; i++)
      {
        int CrystalID = Hit[i]->CrystalID;
        double timestamp = TimeStamp[i];
        for (int j = 0; j < Amplitude[i]->size(); j++)
        {
          RecHit rechit;
          rechit.Clear();
          rechit.setCrystalID(CrystalID);
          rechit.setFrontCenter(TVector3(Para.HitPosX(CrystalID), Para.HitPosY(CrystalID), Para.HitPosZ(CrystalID)));
          double coarseTime = CoarseTime[i]->at(j);
          double fineTime = FineTime[i]->at(j);
          double amp = Amplitude[i]->at(j);
          double time = coarseTime - timestamp;
          rechit.setEnergy(amp, Para.HGNoise(i), Para.HGPedestal(i), Para.HGMipPeak(i));
          rechit.setTime(time);
          if (rechit.Energy() > 0)
          {
            HitMap[CrystalID].push_back(rechit);
            HitID.push_back(rechit.CrystalID());
            HitTime.push_back(rechit.Time());
            HitEnergy.push_back(rechit.Energy());
          }
        }
      }

      Hit2Cluster m_Hit2Cluster;
      m_Hit2Cluster.Convertmulti(HitMap, ClusterMap, timecut);
      Cluster2Shower m_Cluster2Shower;
      m_Cluster2Shower.Convertmulti(ClusterMap, ShowerMap);

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
    }

    rec_data->Fill();
  }

  f->cd();
  rec_data->Write();
  f->Close();
  
  for (int i = 0; i < 25; i++)
  {
    delete Hit[i];
    delete CoarseTime[i];
    delete FineTime[i];
    delete Amplitude[i];
  }

  return 0;
}
