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

void Hit2Cluster::Convertmulti(const map<int,vector<RecHit>>& HitMap,multimap<int,Cluster>& ClusterMap, TGraph* timecut)
{
  map<int,vector<RecHit>>::const_iterator Hiter;
  list<int> CrystalIDVec;
  CrystalIDVec.clear();
  list<int>::iterator CrystalIDiter;
  map<int,vector<int>> ClusterMemberMap;
  ClusterMemberMap.clear();
  map<int,vector<int>>::iterator CMiter;

  for(Hiter=HitMap.begin();Hiter!=HitMap.end();Hiter++){
    CrystalIDVec.push_back(Hiter->first);
  }

  list<int> CopyCrystalIDVec(CrystalIDVec);
  list<int> BufferCrystalIDVec;
  list<int>::iterator Citer,Biter;

  vector<int> NeighborVec;
  vector<int>::iterator Niter;
  Neighbor Ngh;
  int numCluster=0;

  while(!CopyCrystalIDVec.empty()){
    Citer=CopyCrystalIDVec.begin();
    BufferCrystalIDVec.push_back(*Citer);
    CopyCrystalIDVec.erase(Citer);
  
    while(!BufferCrystalIDVec.empty()){
      Biter=BufferCrystalIDVec.begin();
      NeighborVec=Ngh.GetNeighbors(*Biter);
      for(Niter=NeighborVec.begin();Niter!=NeighborVec.end();Niter++){
        Citer=find(CopyCrystalIDVec.begin(),CopyCrystalIDVec.end(),*Niter);
        if(Citer!=CopyCrystalIDVec.end()){
          BufferCrystalIDVec.push_back(*Citer);
          CopyCrystalIDVec.erase(Citer);
        }
      }
      ClusterMemberMap[numCluster].push_back(*Biter);
      BufferCrystalIDVec.erase(Biter);
    }
    numCluster++;
  }
  for(CMiter=ClusterMemberMap.begin();CMiter!=ClusterMemberMap.end();CMiter++){
    vector<int> CryIDVec=CMiter->second;
    vector<int>::iterator CryIDiter;
  }

  for(CMiter=ClusterMemberMap.begin();CMiter!=ClusterMemberMap.end();CMiter++){
    int ClusterIDint=CMiter->first;
    vector<int> CryIDVec=CMiter->second;
    vector<int>::iterator CryIDiter;
    
    map<int,list<RecHit>> SubHitMap;
    map<int,RecHit> SubBufferHitMap;
    map<int,list<RecHit>>::iterator SubHiter;
    map<int,RecHit>::iterator SubBiter;
    for(CryIDiter=CryIDVec.begin();CryIDiter!=CryIDVec.end();CryIDiter++){
      Hiter=HitMap.find(*CryIDiter);
      if(Hiter!=HitMap.end()){
        list<RecHit> SubHitList(Hiter->second.begin(),Hiter->second.end());
        SubHitMap.insert({Hiter->first,SubHitList});
      }
    }
    
    while(!SubHitMap.empty()){
      double ECluster=0; //total energy of a cluster
      double EMax=0; //maximum energy in a cluster
      int ClusterID; //ID corresponds to the maximum energy
      int MaxCrystalID;
      list<RecHit>::iterator MaxHititer;
      list<RecHit>::iterator SubViter;
      list<int> SubCrystalIDVec;
      SubCrystalIDVec.clear();
      list<int>::iterator SubCrystalIDiter;
      for(SubHiter=SubHitMap.begin();SubHiter!=SubHitMap.end();SubHiter++){
        SubCrystalIDVec.push_back(SubHiter->first);
        for(SubViter=SubHiter->second.begin();SubViter!=SubHiter->second.end();SubViter++){
          if(SubViter->Energy()>EMax){
            EMax=SubViter->Energy();
            MaxCrystalID=SubHiter->first;
            MaxHititer=SubViter;
          }
        }
      }

      double TSeed=MaxHititer->Time();
      double RSeed=MaxHititer->FrontCenter().Mag();
      SubBufferHitMap.insert({MaxCrystalID, *MaxHititer});
      SubHitMap[MaxCrystalID].erase(MaxHititer);
      if(SubHitMap[MaxCrystalID].empty()){
        SubHitMap.erase(MaxCrystalID);
      }
      SubCrystalIDiter=find(SubCrystalIDVec.begin(),SubCrystalIDVec.end(),MaxCrystalID);
      if(SubCrystalIDiter!=SubCrystalIDVec.end()){
        SubCrystalIDVec.erase(SubCrystalIDiter);
      }
      Cluster aCluster;
      ClusterID=MaxCrystalID;

      while(!SubBufferHitMap.empty()){
        SubBiter=SubBufferHitMap.begin();
        NeighborVec=Ngh.GetNeighbors(SubBiter->first);
        for(Niter=NeighborVec.begin();Niter!=NeighborVec.end();Niter++){
          double SubEMax=0;
          bool iffound=false;
          int SubMaxCrystalID;
          list<RecHit>::iterator SubMaxHititer;
          SubCrystalIDiter=find(SubCrystalIDVec.begin(),SubCrystalIDVec.end(),*Niter);
          if(SubCrystalIDiter==SubCrystalIDVec.end()) continue;
          SubHiter=SubHitMap.find(*Niter);
          if(SubHiter!=SubHitMap.end()){
            for(SubViter=SubHiter->second.begin();SubViter!=SubHiter->second.end();SubViter++){
              double hitenergy=SubViter->Energy();
              double hitr=SubViter->FrontCenter().Mag();
              double dRtime = (hitr - RSeed)/29.9792458;
              double tcut = 100;
              if(hitenergy>0.1){
                tcut = 10;
              }
              else if(hitenergy>0.005){
                tcut = timecut->Eval(hitenergy);
              }
              if(fabs(SubViter->Time()-TSeed-dRtime)<tcut){
                if(SubViter->Energy()>SubEMax){
                  SubEMax=SubViter->Energy();
                  SubMaxCrystalID=SubHiter->first;
                  SubMaxHititer=SubViter;
                  iffound=true;
                }
              }
            }
          }
          if(iffound){
            SubBufferHitMap.insert({SubMaxCrystalID, *SubMaxHititer});
            SubCrystalIDVec.erase(SubCrystalIDiter);
            SubHitMap[SubMaxCrystalID].erase(SubMaxHititer);
            if(SubHitMap[SubMaxCrystalID].empty()){
              SubHitMap.erase(SubMaxCrystalID);
            }
          }
        }
        ECluster+=SubBiter->second.Energy();
        aCluster.Insert(SubBiter->first,SubBiter->second);
        SubBufferHitMap.erase(SubBiter);
      }
      if(ECluster>m_EThresholdCluster){
        aCluster.setClusterID(ClusterID);
        aCluster.setEnergy(ECluster);
        ClusterMap.insert({ClusterID,aCluster});
      }
    }
  } 
  
}
