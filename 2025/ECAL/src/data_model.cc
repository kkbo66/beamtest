#include "data_model.hh"
#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;


decode_data_col::decode_data_col() 
{ 
   clear();
}

decode_data_col::~decode_data_col() { }

bool decode_data_col::clear()
{
    memset(LAmplitude,0,sizeof(LAmplitude));
    memset(HAmplitude,0,sizeof(HAmplitude));
    memset(LNoise,0,sizeof(LNoise));
    memset(HNoise,0,sizeof(HNoise));

    CrystalID=0; 
    Temperature1=0;
    Temperature2=0;
    LowGainPedestal=0;
    HighGainPedestal=0;
    LowGainPeak=0;
    HighGainPeak=0;

   
    return true;
}

void decode_data_col::Set(int crystalID,float tmpera1,float tmpera2,float rowal[256],float rowah[256],float rownl[25],float rownh[25], float mlplat,float mhplat,float mlpeak,float mhpeak)
{
    for(int i=0;i<256;i++){
      LAmplitude[i]=rowal[i];
      HAmplitude[i]=rowah[i];
    }
    
    for(int i=0;i<25;i++){
      LNoise[i]=rownl[i];
      HNoise[i]=rownh[i];
    }
    
    CrystalID=crystalID;
    Temperature1=tmpera1;
    Temperature2=tmpera2;
    
    LowGainPedestal=mlplat;
    HighGainPedestal=mhplat;
    LowGainPeak=mlpeak;
    HighGainPeak=mhpeak;
}

