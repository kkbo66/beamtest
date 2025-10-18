#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TF1.h>

#include <TMath.h>
#include <TString.h>

using namespace std;
using namespace TMath;

void drawCal(TString rootfile = "rootfile.txt", TString elecfile = "elec.txt"){

  ifstream infile;
  infile.open(elecfile, ios::in);
  vector<int> eleclist;
  eleclist.clear();
  string line;
  while(getline(infile, line)){
    eleclist.push_back(stoi(line)); 
  }
  infile.close();
  cout<<"There are "<<eleclist.size()<<" channels to be analyzed."<<endl;
  ifstream infile2;
  infile2.open(rootfile, ios::in);
  vector<TString> rootlist;
  while(getline(infile2, line)){
    rootlist.push_back(line); 
  }
  infile2.close();
  cout<<"There are "<<rootlist.size()<<" root files to be analyzed."<<endl;
  if(rootlist.size()!=eleclist.size()){
    cout<<"The number of root files is not equal to the number of channels!"<<endl;
    return;
  }
  vector<vector<double>> peaklistl;
  peaklistl.clear();
  vector<vector<double>> peaklisth;
  peaklisth.clear();
  for(size_t i=0; i<eleclist.size(); i++){
    double elec = eleclist[i];
    TString rootname = rootlist[i];
    size_t pos = rootname.Last('.');
    size_t pos2 = rootname.Last('/');
    rootname = rootname(pos2+1, pos-pos2-1);
    ifstream infile3;
    infile3.open("/home/kkbo/beamtest/draw/elecal/para/peakvalues_"+rootname+".txt", ios::in);
   
    vector<double> peakrowl;
    peakrowl.clear();
    vector<double> peakrowh;
    peakrowh.clear();
    for(size_t j=0; j<5; j++){
      for(size_t k=0; k<5; k++){
        string dummy;
        double low, high;
        infile3>>dummy;
        infile3>>low;
        infile3>>dummy;
        infile3>>high;
        //infile3>>low>>high;
        peakrowl.push_back(low);
        peakrowh.push_back(high);
      }
    }
    peaklistl.push_back(peakrowl);
    peaklisth.push_back(peakrowh);
    infile3.close();
  }
  TGraph* grl[25];
  TGraph* grh[25];
  for(int i=0; i<25; i++){
    grl[i] = new TGraph(eleclist.size());
    grh[i] = new TGraph(eleclist.size());
    grl[i]->SetTitle(Form("Low Peak Channel %02d;Elec Channel;Low Peak (ADC)", i+1));
    grh[i]->SetTitle(Form("High Peak Channel %02d;Elec Channel;High Peak (ADC)", i+1));
    grl[i]->SetMarkerStyle(7);
    grh[i]->SetMarkerStyle(7);
    grl[i]->SetMarkerColor(kBlue);
    grh[i]->SetMarkerColor(kRed);
    grl[i]->SetMarkerSize(0.8);
    grh[i]->SetMarkerSize(0.8);
    for(size_t j=0; j<eleclist.size(); j++){
      double elec = eleclist[j];
      cout<<"Channel "<<elec<<" Peak "<<i+1<<" Low: "<<peaklistl[j][i]<<" High: "<<peaklisth[j][i]<<endl;
      grl[i]->SetPoint(j, elec, peaklistl[j][i]);
      if(j<eleclist.size()/3*2)
        grh[i]->SetPoint(j, elec, peaklisth[j][i]);
    }
  }
  TCanvas* c1 = new TCanvas("c1","c1",800,600);
  c1->SetGrid();
  c1->Divide(5,5);
  for(int i=0; i<25; i++){
    //c1->cd(i+1);
    c1->cd(21-i%5*5+i/5);
    gPad->SetGrid();
    grl[i]->Draw("AP");
  }
  TCanvas* c2 = new TCanvas("c2","c2",800,600);
  c2->SetGrid();
  c2->Divide(5,5);
  for(int i=0; i<25; i++){
    //c2->cd(i+1);
    c2->cd(21-i%5*5+i/5);
    gPad->SetGrid();
    grh[i]->Draw("AP");
  }
  c1->SaveAs("LowPeaks.pdf");
  c2->SaveAs("HighPeaks.pdf");

}

#if !defined(__CINT__) && !defined(__CLING__)
int main(int argv,char* argc[])
{
  if(argv!=3){
    cout<<"Usage: ./drawCal rootfile.txt elec.txt"<<endl;
    return -1;
  }
  TString rootfile = argc[1];
  TString eleclist = argc[2];
  drawCal(rootfile, eleclist);
  return 0;
}
#endif

