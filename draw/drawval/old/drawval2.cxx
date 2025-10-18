using namespace std;
using namespace TMath;
#include <iostream>
#include <fstream>

#define _Npoints 256
#define _Nnoise 25
// set time window begin&end
#define _timewinb 40
#define _timewine 80

class DataModel2025
{
public:
    DataModel2025();
    ~DataModel2025();
    void Set(int, float, float, float[_Npoints], float[_Npoints], float[_Nnoise], float[_Nnoise], float, float, float, float);

    bool clear();

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
};

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

void drawval2(TString rootname){

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetLegendFont(42);
  gStyle->SetTextFont(42);
  gStyle->SetTitleSize(0.05,"xyz");
  gStyle->SetLabelSize(0.04,"xyz");
  gStyle->SetLegendTextSize(0.04);
  gStyle->SetTitleOffset(1.2,"y");
  gStyle->SetTitleOffset(1.0,"x");

  TFile *f1 = new TFile("/home/kkbo/beamtest/2025/ECAL/build/"+rootname);
  TTree *t1 = (TTree*)f1->Get("decode_data");
  Int_t nentries = (Int_t)t1->GetEntries();
  DataModel2025 *data = new DataModel2025();
  t1->SetBranchAddress("Hit_1_1",data);
  cout << "nentries: " << nentries << endl;
  for(Int_t i=0; i<nentries; i++){
    t1->GetEntry(i);
  }

#if 0
  const Int_t nPoints = 256;
  TH1F *hTemp1[5][5];
  TH1F *hTemp2[5][5];
  TH1F *hLAmpl[5][5];
  TH1F *hHAmpl[5][5];
  TH1F *hLNoise[5][5];
  TH1F *hHNoise[5][5];
  TH1F *hLPed[5][5];
  TH1F *hHPed[5][5];
  TH1F *hLPeak[5][5];
  TH1F *hHPeak[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      //hTemp1[i][j] = new TH1F(Form("hTemp1_%d_%d",i,j),Form("Temperature1 Crystal (%d,%d);Temperature1 [#circC];Counts",i,j),100,-20,50);
      //hTemp2[i][j] = new TH1F(Form("hTemp2_%d_%d",i,j),Form("Temperature2 Crystal (%d,%d);Temperature2 [#circC];Counts",i,j),100,-20,50);
      //hLAmpl[i][j] = new TH1F(Form("hLAmpl_%d_%d",i,j),Form("Low Gain Amplitude Crystal (%d,%d);Low Gain Amplitude [ADC];Counts",i,j),100,0,2000);
      //hHAmpl[i][j] = new TH1F(Form("hHAmpl_%d_%d",i,j),Form("High Gain Amplitude Crystal (%d,%d);High Gain Amplitude [ADC];Counts",i,j),100,0,2000);
      //hLNoise[i][j] = new TH1F(Form("hLNoise_%d_%d",i,j),Form("Low Gain Noise Crystal (%d,%d);Low Gain Noise [ADC];Counts",i,j),100,0,50);
      //hHNoise[i][j] = new TH1F(Form("hHNoise_%d_%d",i,j),Form("High Gain Noise Crystal (%d,%d);High Gain Noise [ADC];Counts",i,j),100,0,50);
      //hLPed[i][j] = new TH1F(Form("hLPed_%d_%d",i,j),Form("Low Gain Pedestal Crystal (%d,%d);Low Gain Pedestal [ADC];Counts",i,j),100,0,200);
      //hHPed[i][j] = new TH1F(Form("hHPed_%d_%d",i,j),Form("High Gain Pedestal Crystal (%d,%d);High Gain Pedestal [ADC];Counts",i,j),100,0,200);
      //hLPeak[i][j] = new TH1F(Form("hLPeak_%d_%d",i,j),Form("Low Gain Peak Crystal (%d,%d);Low Gain Peak [ADC];Counts",i,j),100,0,2000);
      //hHPeak[i][j] = new TH1F(Form("hHPeak_%d_%d",i,j),Form("High Gain Peak Crystal (%d,%d);High Gain Peak [ADC];Counts",i,j),100,0,2000);
    }
  }
  TH2F h2Temp1Time[5][5];
  TH2F *h2Temp2Time[5][5];
  TH2F *h2LAmplTime[5][5];
  TH2F *h2HAmplTime[5][5];
  TH2F *h2LNoiseTime[5][5];
  TH2F *h2HNoiseTime[5][5];
  TH2F *h2LPedTime[5][5];
  TH2F *h2HPedTime[5][5];
  TH2F *h2LPeakTime[5][5];
  TH2F *h2HPeakTime[5][5];
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      //h2Temp1Time[i][j] = new TH2F(Form("h2Temp1Time_%d_%d",i,j),Form("Temperature1 vs Time Crystal (%d,%d);Time [ns];Temperature1 [#circC]",i,j),256,0,3200,100,-20,50);
      //h2Temp2Time[i][j] = new TH2F(Form("h2Temp2Time_%d_%d",i,j),Form("Temperature2 vs Time Crystal (%d,%d);Time [ns];Temperature2 [#circC]",i,j),256,0,3200,100,-20,50);
      //h2LAmplTime[i][j] = new TH2F(Form("h2LAmplTime_%d_%d",i,j),Form("Low Gain Amplitude vs Time Crystal (%d,%d);Time [ns];Low Gain Amplitude [ADC]",i,j),256,0,3200,100,0,2000);
      //h2HAmplTime[i][j] = new TH2F(Form("h2HAmplTime_%d_%d",i,j),Form("High Gain Amplitude vs Time Crystal (%d,%d);Time [ns];High Gain Amplitude [ADC]",i,j),256,0,3200,100,0,2000);
      //h2LNoiseTime[i][j] = new TH2F(Form("h2LNoiseTime_%d_%d",i,j),Form("Low Gain Noise vs Time Crystal (%d,%d);Time [ns];Low Gain Noise [ADC]",i,j),256,0,3200,100,0,50);
      //h2HNoiseTime[i][j] = new TH2F(Form("h2HNoiseTime_%d_%d",i,j),Form("High Gain Noise vs Time Crystal (%d,%d);Time [ns];High Gain Noise [ADC]",i,j),256,0,3200,100,0,50);
      //h2LPedTime[i][j] = new TH2F(Form("h2LPedTime_%d_%d",i,j),Form("Low Gain Pedestal vs Time Crystal (%d,%d);Time [ns];Low Gain Pedestal [ADC]",i,j),256,0,3200,100,0,200);
      //h2HPedTime[i][j] = new TH2F(Form("h2HPedTime_%d_%d",i,j),Form("High Gain Pedestal vs Time Crystal (%d,%d);Time [ns];High Gain Pedestal [ADC]",i,j),256,0,3200,100,0,200);
      //h2LPeakTime[i][j] = new TH2F(Form("h2LPeakTime_%d_%d",i,j),Form("Low Gain Peak vs Time Crystal (%d,%d);Time [ns];Low Gain Peak [ADC]",i,j),256,0,3200,100,0,2000);
      //h2HPeakTime[i][j] = new TH2F(Form("h2HPeakTime_%d_%d",i,j),Form("High Gain Peak vs Time Crystal (%d,%d);Time [ns];High Gain Peak [ADC]",i,j),256,0,3200,100,0,2000);
    }
  } 
  
  for(Int_t i=0; i<5; i++){
    for(Int_t j=0; j<5; j++){
      Int_t triggerID;
      Double_t Time[nPoints];
      Long64_t CrystalID;
      Double_t Temperature1[nPoints];
      Double_t Temperature2[nPoints];
      Double_t LAmplitude[nPoints];
      Double_t HAmplitude[nPoints];
      Double_t LNoise[nPoints];
      Double_t HNoise[nPoints];
      Double_t LowGainPedestal[nPoints];
      Double_t HighGainPedestal[nPoints];
      Double_t LowGainPeak[nPoints];
      Double_t HighGainPeak[nPoints];
      t1->SetBranchAddress(Form("Hit_%d_%d.TriggerID",i+1,j+1),&triggerID);
      t1->SetBranchAddress(Form("Hit_%d_%d.Time",i+1,j+1),&Time);
      t1->SetBranchAddress(Form("Hit_%d_%d.CrystalID",i+1,j+1),&CrystalID);
      t1->SetBranchAddress(Form("Hit_%d_%d.Temperature1",i+1,j+1),Temperature1);
      t1->SetBranchAddress(Form("Hit_%d_%d.Temperature2",i+1,j+1),Temperature2);
      t1->SetBranchAddress(Form("Hit_%d_%d.LAmplitude",i+1,j+1),LAmplitude);
      t1->SetBranchAddress(Form("Hit_%d_%d.HAmplitude",i+1,j+1),HAmplitude);
      t1->SetBranchAddress(Form("Hit_%d_%d.LNoise",i+1,j+1),LNoise);
      t1->SetBranchAddress(Form("Hit_%d_%d.HNoise",i+1,j+1),HNoise);
      t1->SetBranchAddress(Form("Hit_%d_%d.LowGainPedestal",i+1,j+1),LowGainPedestal);
      t1->SetBranchAddress(Form("Hit_%d_%d.HighGainPedestal",i+1,j+1),HighGainPedestal);
      t1->SetBranchAddress(Form("Hit_%d_%d.LowGainPeak",i+1,j+1),LowGainPeak);
      t1->SetBranchAddress(Form("Hit_%d_%d.HighGainPeak",i+1,j+1),HighGainPeak);

      ofstream outtxt;
      outtxt.open(Form("Hit_%d_%d.txt",i,j));
      for(Int_t k=0; k<nentries; k++){
        t1->GetEntry(k);
        if(k!=0) continue; // For testing, only process the first entry 
        outtxt << "Entry: " << k 
               << " CrystalID: " << CrystalID; 
        for(Int_t p=0; p<nPoints; p++){
          outtxt << Form(" Time[%d]: %.1f",p,Time[p])
                 << Form(" Temp1[%d]: %.2f",p,Temperature1[p])
                 << Form(" Temp2[%d]: %.2f",p,Temperature2[p])
                 << Form(" LAmpl[%d]: %.2f",p,LAmplitude[p])
                 << Form(" HAmpl[%d]: %.2f",p,HAmplitude[p])
                 << Form(" LNoise[%d]: %.2f",p,LNoise[p])
                 << Form(" HNoise[%d]: %.2f",p,HNoise[p])
                 << Form(" LPed[%d]: %.2f",p,LowGainPedestal[p])
                 << Form(" HPed[%d]: %.2f",p,HighGainPedestal[p])
                 << Form(" LPeak[%d]: %.2f",p,LowGainPeak[p])
                 << Form(" HPeak[%d]: %.2f",p,HighGainPeak[p]) << endl;
        }
        
        //hTemp1[i][j]->Fill(Temperature1[k]);
        //hTemp1Time[i][j]->Fill(Time[k],Temperature1[k]);
        //hTemp2[i][j]->Fill(Temperature2[k]);
        //hTemp2Time[i][j]->Fill(Time[k],Temperature2[k]);
        //hLAmpl[i][j]->Fill(LAmplitude[k]);
        //hLAmplTime[i][j]->Fill(Time[k],LAmplitude[k]);
        //hHAmpl[i][j]->Fill(HAmplitude[k]);  
        //hHAmplTime[i][j]->Fill(Time[k],HAmplitude[k]);
        //hLNoise[i][j]->Fill(LNoise[k]);
        //hLNoiseTime[i][j]->Fill(Time[k],LNoise[k]);
        //hHNoise[i][j]->Fill(HNoise[k]);
        //hHNoiseTime[i][j]->Fill(Time[k],HNoise[k]);
        //hLPed[i][j]->Fill(LowGainPedestal[k]);
        //hLPedTime[i][j]->Fill(Time[k],LowGainPedestal[k]);
        //hHPed[i][j]->Fill(HighGainPedestal[k]);
        //hHPedTime[i][j]->Fill(Time[k],HighGainPedestal[k]);
        //hLPeak[i][j]->Fill(LowGainPeak[k]);
        //hLPeakTime[i][j]->Fill(Time[k],LowGainPeak[k]);
        //hHPeak[i][j]->Fill(HighGainPeak[k]);
        //hHPeakTime[i][j]->Fill(Time[k],HighGainPeak[k]);  
      }
    }
  }
#endif

}
