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

  TChain *tree = new TChain("ECALdigi_data");

  for (size_t i = 0; i < datafiles.size(); i++)
    tree->AddFile(datafiles.at(i).data());

  int TriggerID;
  vector<int> *CrystalID = 0;
  vector<double> *TimeHG = 0;
  vector<double> *TimeLG = 0;
  vector<double> *AmpHG_Peak = 0;
  vector<double> *AmpLG_Peak = 0;
  vector<double> *AmpHG_Fit = 0;
  vector<double> *AmpLG_Fit = 0;

  tree->SetBranchAddress("EventID", &TriggerID);
  tree->SetBranchAddress("CrystalID", &CrystalID);
  tree->SetBranchAddress("TimeHG", &TimeHG);
  tree->SetBranchAddress("TimeLG", &TimeLG);
  tree->SetBranchAddress("AmpHG_Peak", &AmpHG_Peak);
  tree->SetBranchAddress("AmpLG_Peak", &AmpLG_Peak);
  tree->SetBranchAddress("AmpHG_Fit", &AmpHG_Fit);
  tree->SetBranchAddress("AmpLG_Fit", &AmpLG_Fit);

  TString outputfile;
  if (argc >= 3)
    outputfile = argv[2];
  else
  {
    outputfile = "Reconstruction.root";
    cout << "Auto save file as Reconstruction.root..." << endl;
  }

  TFile *f = new TFile(outputfile.Data(), "recreate");

  bool IsFitAmp = false;
  if (argc == 4)
    IsFitAmp = true;

  int triggerID;
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
  rec_data->Branch("EventID", &triggerID, "triggerID/I");
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

  TRandom3 r1;
  r1.SetSeed(time(0));

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
      rechit.setCrystalID(CrystalID->at(i));
      rechit.setFrontCenter(TVector3(Para.HitPosX(CrystalID->at(i)), Para.HitPosY(CrystalID->at(i)), Para.HitPosZ(CrystalID->at(i))));
      if (IsFitAmp)
        rechit.setEnergy(AmpHG_Fit->at(i), AmpLG_Fit->at(i), Para.Ratio(i), Para.HGSatuPoint(i), Para.HGNoise(i), Para.LGNoise(i), Para.HGMipPeak(i), Para.HGPedestal(i));
      else
        rechit.setEnergy(AmpHG_Peak->at(i), AmpLG_Peak->at(i), Para.Ratio(i), Para.HGSatuPoint(i), Para.HGPedestal(i), Para.HGNoise(i), Para.LGPedestal(i), Para.LGNoise(i), Para.LGMipPeak(i), Para.HGMipPeak(i), r1);
      rechit.setTime(TimeHG->at(i), TimeLG->at(i), AmpHG_Peak->at(i), Para.LGPedestal(i), Para.HGSatuPoint(i));

      if (rechit.Energy() > 0)
      {
        HitMap[CrystalID->at(i)] = rechit;
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
