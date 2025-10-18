#ifndef DATA_MODEL_HH
#define DATA_MODEL_HH

#include <array>
#include <vector>
#include "TObject.h"
#include "TROOT.h"

using namespace std;

class decode_data_col
{
public:
    decode_data_col();
    ~decode_data_col();
    void Set(int crystalID,float tmpera1,float tmpera2,float rowal[256],float rowah[256],float rownl[25],float rownh[25], float mlplat,float mhplat,float mlpeak,float mhpeak);

    bool clear();

    // Hit infomation
    Long64_t CrystalID;
    Double_t Temperature1;
    Double_t Temperature2;
    Double_t LAmplitude[256];
    Double_t HAmplitude[256];
    Double_t LNoise[25];
    Double_t HNoise[25];

    Double_t LowGainPedestal;
    Double_t HighGainPedestal;
    Double_t LowGainPeak;
    Double_t HighGainPeak;


   
};

#endif
