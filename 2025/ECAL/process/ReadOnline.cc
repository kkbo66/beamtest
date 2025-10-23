#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH1D.h"
#include "DataModel2025.hh"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
void ReadOnline(TString filename)
{
    TFile *infile = new TFile(filename.Data(), "READ");
    TTree *tr = (TTree *)infile->Get("decode_data");
    double timestamp;
    std::vector<double> *charge = nullptr;
    // DataModel2025 *datamodel = new DataModel2025();
    DataModel2025 datamodel;
    tr->SetBranchAddress("Hit_1_1", &datamodel);
    tr->SetBranchAddress("Hit_1_1.Amplitude", &charge);
    TH1D *his = new TH1D("his", "his", 100, -10000, 10000);
    for (int i = 0; i < tr->GetEntries(); i++)
    {
        tr->GetEntry(i);
        if (charge->size() != 1 || datamodel.HighGainPeak > 16000)
            continue;
        his->Fill(charge->at(0) - (datamodel.HighGainPeak - datamodel.HighGainPedestal));
    }
    TCanvas *can = new TCanvas();
    his->Draw();
    can->SaveAs("chargediff.png");
    infile->Close();
}
int main(int argc, char **argv)
{
    if (argc != 2)
        std::cout << "error" << std::endl;
    ReadOnline(argv[1]);
    return 0;
}