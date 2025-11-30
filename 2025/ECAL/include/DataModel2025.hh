#ifndef DATAMODEL2025_HH
#define DATAMODEL2025_HH

#include <array>
#include <vector>
#include "TObject.h"
#include "TROOT.h"

#define _Npoints 256
#define _Nnoise 25
// set time window begin&end
#define _timewinb 120
#define _timewine 180

class DataModel2025
{
public:
    DataModel2025();
    ~DataModel2025();
    void Set(int, float, float, float[_Npoints], float[_Npoints], float[_Nnoise], float[_Nnoise], float, float, float, float);
    bool clear();
    void AddHit(double, double, double, double);

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

#endif
