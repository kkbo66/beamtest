#ifndef SEEDFINDER_HH
#define SEEDFINDER_HH

#include "Cluster.hh"
#include <vector>

using namespace std;

class SeedFinder
{
  public:
    //Constructors and Destructors 
    SeedFinder();
    ~SeedFinder();

    void Seed(const Cluster& aCluster,vector<int>& SeedIDVec);

  private:
    double m_EThresholdSeed;
};
#endif
