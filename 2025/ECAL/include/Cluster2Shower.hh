#ifndef CLUSTER2SHOWER_HH
#define CLUSTER2SHOWER_HH

#include <map>
#include <vector>

#include "Cluster.hh"
#include "Shower.hh"
#include "SeedFinder.hh"
#include "ClusterSplitter.hh"

using namespace std;

class Cluster2Shower
{
  public:
  //Constructors and Destructors
  Cluster2Shower();
  ~Cluster2Shower();

  void Convert(map<int,Cluster>& ClusterMap,map<int,Shower>& ShowerMap);
  void Convertmulti(multimap<int,Cluster>& ClusterMap,multimap<int,Shower>& ShowerMap);

  private:
  SeedFinder* m_SeedFinder;
  ClusterSplitter* m_Splitter;

};
#endif
