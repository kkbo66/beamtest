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
#include <TCanvas.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLine.h>
#include <TLatex.h>
#include <TF1.h>
#include <TMath.h>
#include <TString.h>
#include <TAxis.h>
#include <TPaveStats.h>
#define _Npoints 256
#define _Nnoise 25
// set time window begin&end
#define _timewinb 60
#define _timewine 140

class DataModel2025
{
public:
    // Hit infomation
    Long64_t CrystalID;
    Double_t Temperature1;
    Double_t Temperature2;
    Double_t LAmplitude[_Npoints];
    Double_t HAmplitude[_Npoints];
    Double_t LNoise[_Nnoise];
    Double_t HNoise[_Nnoise];
    Double_t LowGainPedestal;
    Double_t HighGainPedestal;
    Double_t LowGainPeak;
    Double_t HighGainPeak;

    // TQ information
    Double_t TimeStamp;
    std::vector<Double_t> CoarseTime;
    std::vector<Double_t> FineTime;
    std::vector<Double_t> Amplitude;
};