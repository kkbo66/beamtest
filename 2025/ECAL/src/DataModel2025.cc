#include "DataModel2025.hh"
#include <cstring>
#include <iomanip>
#include <iostream>

DataModel2025::DataModel2025()
{

    clear();
}

DataModel2025::~DataModel2025() {}

bool DataModel2025::clear()
{
    memset(LAmplitude, 0, sizeof(LAmplitude));
    memset(HAmplitude, 0, sizeof(HAmplitude));
    memset(LNoise, 0, sizeof(LNoise));
    memset(HNoise, 0, sizeof(HNoise));

    CrystalID = 0;
    Temperature1 = 0;
    Temperature2 = 0;
    LowGainPedestal = 0;
    HighGainPedestal = 0;
    LowGainPeak = 0;
    HighGainPeak = 0;

    TimeStamp = 0;
    CoarseTime.clear();
    FineTime.clear();
    Amplitude.clear();

    return true;
}

void DataModel2025::Set(int crystalID, float tmpera1, float tmpera2, float rowal[_Npoints], float rowah[_Npoints], float rownl[_Nnoise], float rownh[_Nnoise], float mlplat, float mhplat, float mlpeak, float mhpeak)
{
    for (int i = 0; i < _Npoints; i++)
    {
        LAmplitude[i] = rowal[i];
        HAmplitude[i] = rowah[i];
    }

    for (int i = 0; i < _Nnoise; i++)
    {
        LNoise[i] = rownl[i];
        HNoise[i] = rownh[i];
    }

    CrystalID = crystalID;
    Temperature1 = tmpera1;
    Temperature2 = tmpera2;

    LowGainPedestal = mlplat;
    HighGainPedestal = mhplat;
    LowGainPeak = mlpeak;
    HighGainPeak = mhpeak;
}

void DataModel2025::AddHit(double timestamp, double coarsetime, double finetime, double amplitude)
{
    TimeStamp = timestamp;
    CoarseTime.push_back(coarsetime);
    FineTime.push_back(finetime);
    Amplitude.push_back(amplitude);
}
