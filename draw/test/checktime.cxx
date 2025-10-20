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
#include <TH1F.h>
#include <TMath.h>
#include <TString.h>

using namespace std;
using namespace TMath;

void checktime(){

  ifstream infile_ecal("ecal.txt");
  ifstream infile_pidb("pidb.txt");
  ifstream infile_track("track.txt");
  ifstream infile_trigger("trigger.txt");

  vector<double> ecal_time;
  vector<double> pidb_time;
  vector<double> track_time;
  vector<double> trrigger_time;
  vector<int> event_id_pidb;
  vector<int> event_id_track;
  vector<int> event_id_ecal;
  vector<int> event_id_trrigger;
  ecal_time.clear();
  pidb_time.clear();
  track_time.clear();
  trrigger_time.clear();
  event_id_pidb.clear();
  event_id_track.clear();
  event_id_ecal.clear();
  event_id_trrigger.clear();

  double t_ecal, t_pidb, t_track;
  int id_ecal, id_pidb, id_track;
  for(int i=0; i<200; i++){
    string line;
    getline(infile_ecal, line);
    istringstream ss(line);
    string tmp;
    ss >> tmp >> t_ecal >> tmp >> t_ecal;
    id_ecal = i;
    ecal_time.push_back(t_ecal);
    event_id_ecal.push_back(id_ecal);
    getline(infile_ecal, line);
    getline(infile_ecal, line);
    getline(infile_ecal, line);
    getline(infile_ecal, line);
    getline(infile_trigger, line);
    istringstream ss_tr(line);
    string tmp_tr;
    double t_tr;
    ss_tr >> t_tr;
    trrigger_time.push_back(t_tr);
    event_id_trrigger.push_back(id_ecal);
  }

  while(infile_pidb >> t_pidb >> id_pidb){
    pidb_time.push_back(t_pidb);
    event_id_pidb.push_back(id_pidb);
  }
  //while(infile_track >> t_track >> id_track){
  while(infile_track >> id_track >> t_track){
    track_time.push_back(t_track);
    event_id_track.push_back(id_track);
  }

  ofstream outfile1("time_comparison_ecal_pidb.txt");
  int shift1 = 2;
  for(size_t i=0; i<event_id_pidb.size(); i++){
    int id = event_id_pidb[i]+shift1;
    double t_pidb = pidb_time[i];
    for(size_t j=0; j<event_id_ecal.size(); j++){
      if(event_id_ecal[j] == id){
        double t_ecal = ecal_time[j];
        double dt = t_ecal - t_pidb;
        outfile1 << "Event ID: " << id << ", PIDB time: " << setprecision(10) << t_pidb << ", ECAL time: " << setprecision(10) << t_ecal << ", Delta time: " << setprecision(10) << dt << endl;
        break;
      }
    }
  }
  ofstream outfile2("time_comparison_ecal_track.txt");
  int shift2 = 1;
  for(size_t i=0; i<event_id_track.size(); i++){
    int id = event_id_track[i]+shift2;
    double t_track = track_time[i];
    for(size_t j=0; j<event_id_ecal.size(); j++){
      if(event_id_ecal[j] == id){
        double t_ecal = ecal_time[j];
        double dt = t_ecal - t_track;
        outfile2 << "Event ID: " << id << ", Track time: " << setprecision(10) << t_track << ", ECAL time: " << setprecision(10) << t_ecal << ", Delta time: " << setprecision(10) << dt << endl;
        break;
      }
    }
  }

  ofstream outfile3("time_comparison_ecal_trigger.txt");
  int shift3 = 0;
  for(size_t i=0; i<event_id_trrigger.size(); i++){
    int id = event_id_trrigger[i]+shift3;
    double t_tr = trrigger_time[i];
    //if(t_tr < 0.1) continue;
    double t_ecal = ecal_time[i];
    double dt = t_ecal - t_tr;
    outfile3 << "Event ID: " << id << ", Trigger time: " << setprecision(10) << t_tr << ", ECAL time: " << setprecision(10) << t_ecal << ", Delta time: " << setprecision(10) << dt << endl;
  }

  outfile1.close();
  outfile2.close();
  outfile3.close();
}
