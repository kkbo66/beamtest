#include <iostream>
#include <vector>
#include "Hit2Cluster.hh"
#include "Parameter.hh"

using namespace std;

//Constructors and Destructors
Hit2Cluster::Hit2Cluster()
{
  m_EThresholdCluster=Parameter::GetInstance().EThresholdCluster();
  //cout<<m_EThresholdCluster<<endl;
}

Hit2Cluster::~Hit2Cluster()
{

}

void Hit2Cluster::Convert(const map<int,RecHit>& HitMap,map<int,Cluster>& ClusterMap)
{
  map<int,RecHit> CopyHitMap(HitMap);
  map<int,RecHit> BufferHitMap;
  map<int,RecHit>::iterator Citer,Biter;

  vector<int> NeighborVec;
  vector<int>::iterator Niter;

  Neighbor Ngh;

  while(!CopyHitMap.empty()){
    double ECluster=0; //total energy of a cluster
    double EMax=0; //maximum energy in a cluster
    double ClusterID; //ID corresponds to the maximum energy

    Citer=CopyHitMap.begin();
    BufferHitMap.insert({Citer->first, Citer->second});
    CopyHitMap.erase(Citer);
    Cluster aCluster;
    
    while(!BufferHitMap.empty()){
      Biter=BufferHitMap.begin();
      NeighborVec=Ngh.GetNeighbors(Biter->first);
      for(Niter=NeighborVec.begin();Niter!=NeighborVec.end();Niter++){
        Citer=CopyHitMap.find(*Niter);
        if(Citer!=CopyHitMap.end()){
          BufferHitMap.insert({Citer->first, Citer->second});
          CopyHitMap.erase(Citer);
        }
      }

      if(Biter->second.Energy()>EMax){
        EMax=Biter->second.Energy();
        ClusterID=Biter->first; 
      }
      
      ECluster+=Biter->second.Energy();
      aCluster.Insert(Biter->first,Biter->second);
      BufferHitMap.erase(Biter);
    }
    
    if(ECluster>m_EThresholdCluster){
      aCluster.setClusterID(ClusterID);
      aCluster.setEnergy(ECluster);
      ClusterMap.insert({ClusterID,aCluster});
    }
  } 

}
