#ifndef DECODE2025_HH
#define DECODE2025_HH

#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "DataModel2025.hh"
// 1024run26开始样机第1、2、3列电子学通道顺序接反

class Decode2025
{
public:
    Decode2025(std::string);
    ~Decode2025();
    // 直传格式
    void GetHit(std::ifstream &);
    // DAQ打包格式
    void GetHitDAQ(std::ifstream &);
    // 在线处理格式
    void GetHitOnline(std::ifstream &);
    // 调换通道顺序
    void InvertChannel(bool invert) { mChannelInvert = invert; }

private:
    // report error and exit
    void throw_error(std::string);
    // clear hit data
    bool clear(float (&)[6][_Npoints], float (&)[6][_Npoints], float (&)[6][_Nnoise], float (&)[6][_Nnoise], float (&)[6], float (&)[6], float (&)[6], float (&)[6]);
    // read work state information
    bool ReadState(std::ifstream &, float (&)[10], float &, float &);
    // read hit data of one block(no work state information)
    bool ReadDataDAQ(std::ifstream &, float (&)[6][_Npoints], float (&)[6][_Npoints], float (&)[6][_Nnoise], float (&)[6][_Nnoise], float (&)[6], float (&)[6], float (&)[6], float (&)[6]);
    // read hit data of one block
    bool ReadData(std::ifstream &, float (&)[6][_Npoints], float (&)[6][_Npoints], float (&)[6][_Nnoise], float (&)[6][_Nnoise], float (&)[6], float (&)[6], float (&)[6], float (&)[6]);
    // read online processed information
    bool ReadData(std::ifstream &, double &, std::vector<unsigned short> &, std::vector<double> &, std::vector<double> &, std::vector<double> &);

    TFile *mFile;
    TTree *mTree;

    std::vector<DataModel2025 *> mHit;

    Long64_t mEventID;
    int mTriggerID;
    Long64_t mTimeCode;
    float mTime[_Npoints];
    float mVoltage[5];
    float mCurrent[5];
    float mTemperature[10];
    // true：更换1、2、3列电子学通道顺序
    bool mChannelInvert;
};

#endif
