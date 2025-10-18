#ifndef DECODE_HH
#define DECODE_HH

#include <string>
#include <TFile.h>
#include "TString.h"
#include <TTree.h>
#include "data_model.hh"

using namespace std;

class Decode
{
  public:
    Decode(string filename);
    ~Decode();
    void GetHit(ifstream& indata);

  private:
    vector<decode_data_col*> Hit;

    Long64_t EventID;
    int TriggerID;
    Long64_t TimeCode;
    float Time[256]; 
    float Voltage[5];
    float Current[5];
    float Temperature[10];

    TFile* f;
    TTree* T;
};

#endif
