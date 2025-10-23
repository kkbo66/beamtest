#ifndef CLUSTERSPLITTER_HH
#define CLUSTERSPLITTER_HH

#include <map>
#include <vector>

#include "Cluster.hh"
#include "Shower.hh"
#include "ShowerPosition.hh"
#include "ShowerEnergy.hh"
#include "ShowerShape.hh"

using namespace std;

class ClusterSplitter
{
  public:
  //Constructors and Destructors
  ClusterSplitter();
  ~ClusterSplitter();

  public:
  void Split(Cluster& aCluster,const vector<int>& SeedIDVec,map<int,Shower>& ShowerMap);
  void Splitmulti(Cluster& aCluster,const vector<int>& SeedIDVec,multimap<int,Shower>& ShowerMap);

  ShowerEnergy* m_ShowerE;
  ShowerPosition* m_ShowerPos;
  ShowerShape* m_ShowerShape;

};
#endif
