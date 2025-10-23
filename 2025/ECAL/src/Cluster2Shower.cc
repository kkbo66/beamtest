#include <iostream>

#include "Cluster2Shower.hh"

//Constructors and Destructors
Cluster2Shower::Cluster2Shower()
{
  m_SeedFinder=new SeedFinder();
  m_Splitter=new ClusterSplitter();
}

Cluster2Shower::~Cluster2Shower()
{
  delete m_SeedFinder;
  delete m_Splitter;
}

void Cluster2Shower::Convert(map<int,Cluster>& ClusterMap,map<int,Shower>& ShowerMap)
{
  map<int,Cluster>::iterator iter;
  vector<int> SeedIDVec;
  vector<int>::const_iterator citer;

  for(iter=ClusterMap.begin();iter!=ClusterMap.end();iter++){
    m_SeedFinder->Seed(iter->second,SeedIDVec);

    if(!SeedIDVec.empty()){
      for(citer=SeedIDVec.begin();citer!=SeedIDVec.end();citer++){
        iter->second.InsertSeed(*citer,iter->second.Find(*citer)->second);
      }
    }

    m_Splitter->Split(iter->second,SeedIDVec,ShowerMap);
    //cout<<ShowerMap.size()<<endl;
  }
}

void Cluster2Shower::Convertmulti(multimap<int,Cluster>& ClusterMap,multimap<int,Shower>& ShowerMap)
{
  multimap<int,Cluster>::iterator iter;
  vector<int> SeedIDVec;
  vector<int>::const_iterator citer;

  for(iter=ClusterMap.begin();iter!=ClusterMap.end();iter++){
    m_SeedFinder->Seed(iter->second,SeedIDVec);
    if(!SeedIDVec.empty()){
      for(citer=SeedIDVec.begin();citer!=SeedIDVec.end();citer++){
        iter->second.InsertSeed(*citer,iter->second.Find(*citer)->second);
      }
    }
    m_Splitter->Splitmulti(iter->second,SeedIDVec,ShowerMap);
    //cout<<ShowerMap.size()<<endl;
  }
}

