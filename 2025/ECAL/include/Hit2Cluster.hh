#ifndef HIT2CLUSTER_HH
#define HIT2CLUSTER_HH

#include <map>
#include "Neighbor.hh"
#include "Cluster.hh"
#include "data_model.hh"
#include "RecHit.hh"
#include <TGraph.h>
#include <list>

using namespace std;

class Hit2Cluster
{
 public:
  //Constructors and Destructors
  Hit2Cluster();
  ~Hit2Cluster();
  
  void Convert(const map<int,RecHit>&,map<int,Cluster>&);
  void Convertmulti(const map<int,vector<RecHit>>&,multimap<int,Cluster>&, TGraph*);

 private:
 double m_EThresholdCluster;

};
#endif
