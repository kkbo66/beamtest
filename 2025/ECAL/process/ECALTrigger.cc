#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input_file>" << endl;
    }

    ifstream infile(argv[1],ios::in|ios::binary);
    if (!infile.is_open()) {
        cout << "Error opening file: " << argv[1] << endl;
        return 1;
    }
    unsigned short *trigger=new unsigned short;
    unsigned int *time1=new unsigned int;
    unsigned int *time2=new unsigned int;
    int timeDiff;
    std::vector<double> timeDiffs;
    int nevents=0;
    int n=0;
    while(!infile.eof()&& infile.good())
    {
      infile.seekg(16,infile.cur);
      infile.read((char*)trigger,1);
      infile.read((char*)time1,sizeof(unsigned int));
      infile.seekg(3,infile.cur);
      infile.read((char*)time2,sizeof(unsigned int));
      infile.seekg(4,infile.cur);
      timeDiff=static_cast<long>(*time1)-(*time2);
      //if(*trigger == 0x33){
      if(true){
        nevents++;
        double timediff=static_cast<double>(timeDiff)/40.0; // Convert to us
        //if(timediff<1)
        timeDiffs.push_back(timediff);
        //cout << "Event " << n << ": Time1 = " << *time1 << ", Time2 = " << *time2 << ", Time Difference = " << timeDiff << endl;
        cout << *time2 << endl;
      }
      n++;
    }
    long max, min;
    max=*std::max_element(timeDiffs.begin(),timeDiffs.end());
    min=*std::min_element(timeDiffs.begin(),timeDiffs.end());
    TH1F *timeDiffHist=new TH1F("timeDiffHist","Time Difference Histogram",100,min,max);
    for(auto diff: timeDiffs)
    {
      timeDiffHist->Fill(diff);
    }
    TCanvas *c1=new TCanvas("c1","Time Difference Histogram",800,600);
    timeDiffHist->Draw();
    TFile *outfile=new TFile("timeDiffHist.root","RECREATE");
    timeDiffHist->Write();
    outfile->Close();
    infile.close();
    cout << "Total events processed: " << n << endl;
    cout << "Total trigger events (0x33): " << nevents << endl;
    cout << "Ratio of trigger events to total events: " << static_cast<double>(nevents)/n << endl;
    return 0;
}
