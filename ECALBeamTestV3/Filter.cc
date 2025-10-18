#include <string>
#include <iostream>
#include <fstream>
#include "TString.h"
#include "data_model.hh"
#include <TFile.h>
#include <TProfile.h>
#include <TTree.h>
#include <TH1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TF1.h>
#include "TChain.h"

using namespace std;

bool isTextFile(const string& fileName){
  size_t dotPos = fileName.find_last_of('.');
  if(dotPos == std::string::npos) return false;

  std::string extension = fileName.substr(dotPos + 1);
  if(extension == "txt")
    return true;
  else 
    return false;
}

int main(int argc, char const *argv[]){

  vector<string> datafiles;

  if(isTextFile(argv[1])){
    ifstream filestream(argv[1]);
    if(filestream.is_open()){
      string filename;
      while(getline(filestream,filename)) datafiles.push_back(filename);
    }
    else cout<< "error in reading filelist  "<<endl;
  }
  else datafiles.push_back(argv[1]);

  TChain *tree = new TChain("decode_data");
  for(size_t i=0;i<datafiles.size();i++) tree->AddFile(datafiles.at(i).data());
  vector<decode_data_col*> Hit(25);
  string Name[25];
  for(int i=0;i<25;i++){
    int no;
    if(i%5==0) no=3; 
    if(i%5==1) no=4; 
    if(i%5==2) no=5; 
    if(i%5==3) no=1; 
    if(i%5==4) no=2; 
    Name[i]=to_string(i/5+1)+"_"+to_string(no);
  }
  for(int i=0;i<25;i++) Hit[i]= new decode_data_col();
  for(int i=0;i<25;i++){
    string name="Hit_"+Name[i];
    tree->SetBranchAddress(name.data(),Hit[i]);
  }

  TString outputfile;
  if(argc==3)
    outputfile = argv[2];
  else{  
    outputfile = "Graph.root";
    cout << "Auto save file as Graph.root..." << endl;
  }

  TFile *f = new TFile(outputfile.Data(),"recreate");

  TH1D *h1[25];
  TH1D *h2[25];
  TH1D *h3[25];
  TH1D *h4[25];
  TH1D *h5[25];
  TH1D *h6[25];
  TGraph *gr[25];
  TCanvas *c1[25];
  TCanvas *c2[25];
  TCanvas *c3[25];
  TCanvas *c4[25];
  TCanvas *c5[25];
  //TProfile *pf[25];
  //TCanvas *c[25];
  //TF1* fun = new TF1("fun","[0]*x*(x<[2])+[1]*(x<[2])+[3]*(x>=[2])",2350,3200);
  //TF1* fun = new TF1("fun","[0]*x*(x<[2])+[1]*(x<[2])+([0]*[2]+[1])*(x>=[2])",2350,3200);
  //fun->SetParameters(25,-55000,2900,16000);
  
  for(int k=0;k<25;k++){
    string histname1="Hit_"+Name[k]+"_LowGainNoise";
    string histname2="Hit_"+Name[k]+"_HighGainNoise";
    string histname3="Hit_"+Name[k]+"_LowGainSignal";
    string histname4="Hit_"+Name[k]+"_HighGainSignal";
    string histname5="Hit_"+Name[k]+"_LowGainPedestal";
    string histname6="Hit_"+Name[k]+"_HighGainPedestal";
    string graphname="Hit_"+Name[k]+"_High-Low_GainRatio";

    h1[k] = new TH1D(histname1.data(),"",100,2250,2450);
    h2[k] = new TH1D(histname2.data(),"",100,1600,3000);
    h3[k] = new TH1D(histname3.data(),"",100,2700,5000);
    h4[k] = new TH1D(histname4.data(),"",100,1600,17000);
    h5[k] = new TH1D(histname5.data(),"",100,2250,2450);
    h6[k] = new TH1D(histname6.data(),"",100,1600,3000);

    h1[k]->GetXaxis()->SetTitle("ADC Counts");
    h1[k]->GetYaxis()->SetTitle("Events");
    h2[k]->GetXaxis()->SetTitle("ADC Counts");
    h2[k]->GetYaxis()->SetTitle("Events");
    h3[k]->GetXaxis()->SetTitle("ADC Counts");
    h3[k]->GetYaxis()->SetTitle("Events");
    h4[k]->GetXaxis()->SetTitle("ADC Counts");
    h4[k]->GetYaxis()->SetTitle("Events");
    h5[k]->GetXaxis()->SetTitle("ADC Counts");
    h5[k]->GetYaxis()->SetTitle("Events");
    h6[k]->GetXaxis()->SetTitle("ADC Counts");
    h6[k]->GetYaxis()->SetTitle("Events");


    gr[k] = new TGraph();
    gr[k]->SetName(graphname.data());
    gr[k]->SetTitle(graphname.data());
    gr[k]->GetXaxis()->SetTitle("Low Gain Peak");
    gr[k]->GetYaxis()->SetTitle("High Gain Peak");

    string canvasname1="Hit_"+Name[k]+"_LowGainNoise";
    string canvasname2="Hit_"+Name[k]+"_HighGainNoise";
    string canvasname3="Hit_"+Name[k]+"_LowGainSignal";
    string canvasname4="Hit_"+Name[k]+"_HighGainSignal";
    string canvasname5="Hit_"+Name[k]+"_High-Low_GainRatio";

    c1[k] = new TCanvas(canvasname1.data(),"",800,600);
    c1[k]->SetGrid();
    c2[k] = new TCanvas(canvasname2.data(),"",800,600);
    c2[k]->SetGrid();
    c3[k] = new TCanvas(canvasname3.data(),"",800,600);
    c3[k]->SetGrid();
    c4[k] = new TCanvas(canvasname4.data(),"",800,600);
    c4[k]->SetGrid();
    c5[k] = new TCanvas(canvasname5.data(),"",800,600);
    c5[k]->SetGrid();

    //string name="Hit_"+to_string(k+1)+"_Profile";
    //pf[k]= new  TProfile(name.data(),"",20,0,7000);
    //c[k]=new TCanvas(name.data(),"",800,600);
  }


  int nEntries=tree->GetEntries();
  int interval = nEntries/20;
  for(int i=0;i<nEntries;i++){

    int progress = static_cast<float>(i+1)/nEntries*100;
    if((i+1)%interval==0){
      cout<<"Progress: "<< progress+1<<"%\r"<<endl;
      std::cout.flush();
    }

    tree->GetEntry(i);

    int count=0;
    int ID=0;
    for(int k=0;k<25;k++){
      if(Hit[k]->HighGainPeak>3500) count++;
      if(Hit[k]->HighGainPeak>3500) ID=k;
      
    }

    if(count==1){
    for(int k=ID;k<=ID;k++){

      for(int j=0;j<25;j++) h1[k]->Fill(Hit[k]->LAmplitude[j]);
      for(int j=0;j<25;j++) h2[k]->Fill(Hit[k]->HAmplitude[j]);
      h3[k]->Fill(Hit[k]->LowGainPeak);
      h4[k]->Fill(Hit[k]->HighGainPeak);
      h5[k]->Fill(Hit[k]->LowGainPedestal);
      h6[k]->Fill(Hit[k]->HighGainPedestal);

      if(Hit[k]->HighGainPeak>2800) gr[k]->SetPoint(gr[k]->GetN(),Hit[k]->LowGainPeak,Hit[k]->HighGainPeak);
      //if(Hit[k]->HighGainPeak>2800) gr[k]->SetPoint(gr[k]->GetN(),Hit[k]->LowGainPeak-2350,Hit[k]->HighGainPeak-2350);

      //if(Hit[k]->HighGainPeak>3000) pf[k]->Fill(Hit[k]->EventID,Hit[k]->HighGainPeak-2300);
    }
    }
  }

  TCanvas *canvas1 = new TCanvas("HighGain_5X5_Noise","",800,600);
  canvas1->Divide(5,5);
  TCanvas *canvas2 = new TCanvas("HighGain_5X5_Signal","",800,600);
  canvas2->Divide(5,5);
  TCanvas *canvas3 = new TCanvas("LowGain_5X5_Noise","",800,600);
  canvas3->Divide(5,5);
  TCanvas *canvas4 = new TCanvas("LowGain_5X5_Signal","",800,600);
  canvas4->Divide(5,5);
  TCanvas *canvas5 = new TCanvas("High-Low_5X5_GainRatio","",800,600);
  canvas5->Divide(5,5);

  for(int k=0;k<25;k++){
    c1[k]->cd();
    h1[k]->Draw();
    c2[k]->cd();
    h2[k]->Draw();
    c3[k]->cd();
    h3[k]->Draw();
    c4[k]->cd();
    h4[k]->Draw();
    c5[k]->cd();
    if(gr[k]->GetN()>0) {
      gr[k]->Draw("AP");
      //gr[k]->Fit(fun,"ESR");
      //fun->Draw("same");
    }

    
    //c[k]->cd();
    //pf[k]->Draw();
    //
    
    int K;
    if(k<5) K=2+(k%5)*5; 
    else if(k<10) K=1+(k%5)*5; 
    else if(k<15) K=0+(k%5)*5; 
    else if(k<20) K=4+(k%5)*5; 
    else if(k<25) K=3+(k%5)*5;   

    canvas1->cd(k+1);
    TPad *pad = new TPad(Form("hg_noise%d",k),"",0.1,0.1,0.9,0.9);
    pad->Draw();
    h2[K]->Draw();

    canvas2->cd(k+1);
    TPad *pad1 = new TPad(Form("hg_signal%d",k),"",0.1,0.1,0.9,0.9);
    pad1->Draw();
    pad1->cd();
    //pad1->SetLogy();
    h4[K]->Draw();
    
    canvas3->cd(k+1);
    TPad *pad2 = new TPad(Form("lg_noise%d",k),"",0.1,0.1,0.9,0.9);
    pad2->Draw();
    h1[K]->Draw();

    canvas4->cd(k+1);
    TPad *pad3 = new TPad(Form("lg_signal%d",k),"",0.1,0.1,0.9,0.9);
    pad3->Draw();
    pad3->cd();
    //pad3->SetLogy();
    h3[K]->Draw();
    
    canvas5->cd(k+1);
    TPad *pad4 = new TPad(Form("gainratio%d",k),"",0.1,0.1,0.9,0.9);
    pad4->Draw();
    if(gr[K]->GetN()>0) gr[K]->Draw("AP");

    f->cd();
    h1[k]->Write();
    h2[k]->Write();
    h3[k]->Write();
    h4[k]->Write();
    h5[k]->Write();
    h6[k]->Write();
    gr[k]->Write();
    //c5[k]->Write();
    //c[k]->Write();
  }

  canvas1->Update();
  canvas2->Update();
  canvas3->Update();
  canvas4->Update();
  canvas5->Update();
  f->cd();
  canvas1->Write();
  canvas2->Write();
  canvas3->Write();
  canvas4->Write();
  canvas5->Write();
  f->Close();

  return 0;
}
