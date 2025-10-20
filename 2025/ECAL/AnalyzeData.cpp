#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <limits.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TROOT.h"
#include "TMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TString.h"
#include "TSpectrum.h"
#include "TPaveStats.h"
#include "TStyle.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TList.h"
#include "TSystem.h"

typedef struct
{
    int triggerID;
    std::vector<int> *HitID = nullptr;
    std::vector<double> *HitEnergy = nullptr;
    std::vector<double> *HitTime = nullptr;
    std::vector<int> *ClusterID = nullptr;
    std::vector<int> *ClusterSize = nullptr;
    std::vector<double> *ClusterEnergy = nullptr;
    std::vector<double> *ClusterSMoment = nullptr;
    std::vector<double> *ClusterPosX = nullptr;
    std::vector<double> *ClusterPosY = nullptr;
    std::vector<double> *ClusterPosZ = nullptr;
    std::vector<int> *ShowerID = nullptr;
    std::vector<double> *ShowerE3x3 = nullptr;
    std::vector<double> *ShowerE5x5 = nullptr;
    std::vector<double> *ShowerEAll = nullptr;
    std::vector<double> *ShowerPosX3x3 = nullptr;
    std::vector<double> *ShowerPosY3x3 = nullptr;
    std::vector<double> *ShowerPosZ3x3 = nullptr;
    std::vector<double> *ShowerPosX5x5 = nullptr;
    std::vector<double> *ShowerPosY5x5 = nullptr;
    std::vector<double> *ShowerPosZ5x5 = nullptr;
    std::vector<double> *ShowerSMoment = nullptr;
    std::vector<double> *ShowerLatMoment = nullptr;
    std::vector<double> *ShowerA20Moment = nullptr;
    std::vector<double> *ShowerA42Moment = nullptr;
} RECONSTRUCT_DATA;
void AnalyzeData(TString filename)
{
    TFile *infile = TFile::Open(filename.Data(), "READ");
    if (!infile->IsOpen())
    {
        std::cerr << "Error! can't open file " << filename << std::endl;
        return;
    }
    RECONSTRUCT_DATA rec_data;
    TTree *rec_tr = (TTree *)infile->Get("rec_data");
    {
        rec_tr->SetBranchAddress("EventID", &rec_data.triggerID);
        rec_tr->SetBranchAddress("HitID", &rec_data.HitID);
        rec_tr->SetBranchAddress("HitEnergy", &rec_data.HitEnergy);
        rec_tr->SetBranchAddress("HitTime", &rec_data.HitTime);
        rec_tr->SetBranchAddress("ClusterID", &rec_data.ClusterID);
        rec_tr->SetBranchAddress("ClusterSize", &rec_data.ClusterSize);
        rec_tr->SetBranchAddress("ClusterEnergy", &rec_data.ClusterEnergy);
        rec_tr->SetBranchAddress("ClusterSMoment", &rec_data.ClusterSMoment);
        rec_tr->SetBranchAddress("ClusterPosX", &rec_data.ClusterPosX);
        rec_tr->SetBranchAddress("ClusterPosY", &rec_data.ClusterPosY);
        rec_tr->SetBranchAddress("ClusterPosZ", &rec_data.ClusterPosZ);
        rec_tr->SetBranchAddress("ShowerID", &rec_data.ShowerID);
        rec_tr->SetBranchAddress("ShowerE3x3", &rec_data.ShowerE3x3);
        rec_tr->SetBranchAddress("ShowerE5x5", &rec_data.ShowerE5x5);
        rec_tr->SetBranchAddress("ShowerEAll", &rec_data.ShowerEAll);
        rec_tr->SetBranchAddress("ShowerPosX3x3", &rec_data.ShowerPosX3x3);
        rec_tr->SetBranchAddress("ShowerPosY3x3", &rec_data.ShowerPosY3x3);
        rec_tr->SetBranchAddress("ShowerPosZ3x3", &rec_data.ShowerPosZ3x3);
        rec_tr->SetBranchAddress("ShowerPosX5x5", &rec_data.ShowerPosX5x5);
        rec_tr->SetBranchAddress("ShowerPosY5x5", &rec_data.ShowerPosY5x5);
        rec_tr->SetBranchAddress("ShowerPosZ5x5", &rec_data.ShowerPosZ5x5);
        rec_tr->SetBranchAddress("ShowerSMoment", &rec_data.ShowerSMoment);
        rec_tr->SetBranchAddress("ShowerLatMoment", &rec_data.ShowerLatMoment);
        rec_tr->SetBranchAddress("ShowerA20Moment", &rec_data.ShowerA20Moment);
        rec_tr->SetBranchAddress("ShowerA42Moment", &rec_data.ShowerA42Moment);
    }
    TH1D *total_energy_his = new TH1D();
    total_energy_his->SetDirectory(nullptr);
    total_energy_his->SetNameTitle("his", "total energy deposition;energy/MeV;counts");
    total_energy_his->SetBins(5490, 10, 5500);

    TH1D *hit_channel_his = new TH1D();
    hit_channel_his->SetDirectory(nullptr);
    hit_channel_his->SetNameTitle("his", "hit channel number;channel number;counts");
    hit_channel_his->SetBins(25, 0, 25);

    TH1D *energy_his_channel[25];
    for (int i = 0; i < 25; i++)
    {
        energy_his_channel[i] = new TH1D();
        energy_his_channel[i]->SetDirectory(nullptr);
        energy_his_channel[i]->SetNameTitle(Form("his"), Form("%i channels;enenrgy/MeV;counts", i + 1));
        energy_his_channel[i]->SetBins(5490, 10, 5500);
    }
    double maximum_energy = 0;
    int EventNum = rec_tr->GetEntries();
    int process;
    double energy_sum;
    int hit_num;
    for (int i = 0; i < EventNum; i++)
    {
        if (i % (EventNum / 10) == 0 && i != 0)
        {
            process = 10 * i / (EventNum / 10);
            std::cout << "\rProcessing: >>>>>>>>>>>>>>> " << process << "% <<<<<<<<<<<<<<<";
            std::cout.flush();
        }
        rec_tr->GetEntry(i);
        energy_sum = 0;
        for (std::vector<double>::iterator ite = rec_data.HitEnergy->begin(); ite != rec_data.HitEnergy->end(); ite++)
            energy_sum += *ite;
        if (energy_sum > maximum_energy)
            maximum_energy = energy_sum;
        hit_num = rec_data.HitID->size();
        total_energy_his->Fill(energy_sum);
        hit_channel_his->Fill(hit_num);
        if (hit_num != 0)
            energy_his_channel[hit_num - 1]->Fill(energy_sum);
    }
    std::cout << std::endl;
    std::cout << "maximum energy: " << maximum_energy << std::endl;
    total_energy_his->GetXaxis()->SetRangeUser(0, maximum_energy);
    for (int i = 0; i < 25; i++)
    {
        energy_his_channel[i]->GetXaxis()->SetRangeUser(0, maximum_energy);
    }
    TCanvas *can = new TCanvas("can", "can", 900, 600);
    can->Divide(5, 5);
    for (int i = 0; i < 25; i++)
    {
        can->cd(i + 1);
        energy_his_channel[i]->Draw();
        can->Update();
    }
    total_energy_his->Draw();
    // hit_channel_his->Draw();
}
