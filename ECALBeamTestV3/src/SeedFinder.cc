#include <iostream>
#include <map>
#include "data_model.hh"
#include "RecHit.hh"
#include "Neighbor.hh"
#include "Parameter.hh"
#include "SeedFinder.hh"

//Constructors and Destructors 
SeedFinder::SeedFinder()
{
  m_EThresholdSeed=Parameter::GetInstance().EThresholdSeed();
}

SeedFinder::~SeedFinder()
{

}

void SeedFinder::Seed(const Cluster& aCluster,vector<int>& SeedIDVec)
{
  map<int,RecHit>::const_iterator iter1;
  map<int,RecHit>::const_iterator iter2;
  
  Neighbor Ngh;
  vector<int> NeighborVec;
  vector<int>::const_iterator iter3;
  bool isMax;

  SeedIDVec.clear();

  for(iter1=aCluster.Begin();iter1!=aCluster.End();iter1++){
   isMax=true;
   NeighborVec.clear();
   NeighborVec=Ngh.GetNeighbors(iter1->first);

   for(iter3=NeighborVec.begin();iter3!=NeighborVec.end();iter3++){
     iter2=aCluster.Find(*iter3);
     /*if(iter2==aCluster.End()){
        isMax=false;
        break;
     }*/
     if(iter2!=aCluster.End()){
     if((iter2->second.Energy())>=(iter1->second.Energy()))
        isMax=false;
     }
   }

   if(isMax==true&&(iter1->second.Energy())>m_EThresholdSeed){
     SeedIDVec.push_back(iter1->first);
   }
  }
}
