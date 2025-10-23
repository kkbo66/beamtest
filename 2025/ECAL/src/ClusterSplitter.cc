#include <iostream>

#include "Parameter.hh"
#include "ClusterSplitter.hh"
#include "RecHit.hh"
#include "TVector3.h"
#include "Neighbor.hh"

//Constructors and Destructors
ClusterSplitter::ClusterSplitter()
{
  m_ShowerE = new ShowerEnergy();
  m_ShowerPos = new ShowerPosition();
  m_ShowerShape = new ShowerShape();
}

ClusterSplitter::~ClusterSplitter()
{
  delete m_ShowerE;
  delete m_ShowerPos;
  delete m_ShowerShape;
}

void ClusterSplitter::Split(Cluster& aCluster,const vector<int>& SeedIDVec,map<int,Shower>& ShowerMap)
{
  Parameter& Para=Parameter::GetInstance();

  Shower aShower;
  RecHit aHit;
  map<int,RecHit>::const_iterator citer;

  if(SeedIDVec.size()==0){
    return;
  }
  else if(SeedIDVec.size()==1){
    aShower.Clear();
    aShower.setSeedID(SeedIDVec[0]);
    aShower.setClusterID(aCluster.GetClusterID());

    for(citer=aCluster.Begin();citer!=aCluster.End();citer++){
      aShower.Insert(citer->first,citer->second);
    }

    m_ShowerE->Energy(aShower);
    m_ShowerPos->Position(aShower);
    m_ShowerShape->CalculateMoment(aShower);

    ShowerMap[SeedIDVec[0]]=aShower;
  }
  else{

    unsigned int iterations=0; //Number of iterations
    double PosChange; //Change in position between two iterations

    map<int,TVector3> ShowerPos;
    map<int,TVector3>::const_iterator mciter1;
    map<int,RecHit>::const_iterator mciter2;

    vector<int>::const_iterator vciter;
    vector<int>::const_iterator vciter1;

    for(vciter=SeedIDVec.begin();vciter!=SeedIDVec.end();vciter++){
      mciter2=aCluster.Find(*vciter);
      if(mciter2!=aCluster.End())
      ShowerPos[mciter2->first]=mciter2->second.FrontCenter();
    }
    
    map<int,Shower> tmpShowerMap;
    map<int,Shower>::const_iterator mciter3;
    
    do{
      PosChange=0;
      tmpShowerMap.clear();

      for(vciter=SeedIDVec.begin();vciter!=SeedIDVec.end();vciter++){
        aShower.Clear();
        aShower.setSeedID(*vciter);
        aShower.setClusterID(aCluster.GetClusterID());

        //calculate the weight for each cell for each seed
        for(mciter2=aCluster.Begin();mciter2!=aCluster.End();mciter2++){
          RecHit aHit=mciter2->second;
          double aDistance=0;
          double aESeed=0;
          //bool isSeed=false;
        
          double weightsum=0,weight=0;

          for(vciter1=SeedIDVec.begin();vciter1!=SeedIDVec.end();vciter1++){
            TVector3 ShowerPoint(ShowerPos[*vciter1]);
            TVector3 HitPoint(aHit.FrontCenter());

            double ESeed=aCluster.Find(*vciter1)->second.Energy();
            double Distance;

            if(*vciter1==mciter2->first){
              //isSeed=true;
              Distance=0;
            }
            else{
              double t=(HitPoint.x()*ShowerPoint.x()+HitPoint.y()*ShowerPoint.y()+HitPoint.z()*ShowerPoint.z())/(ShowerPoint.x()*ShowerPoint.x()+ShowerPoint.y()*ShowerPoint.y()+ShowerPoint.z()*ShowerPoint.z());
              Distance=(HitPoint-t*ShowerPoint).Mag();
              //Distance=(HitPoint-ShowerPoint).Mag();
          //if(mciter2->first==310132) cout<<ESeed<<"  "<<Distance<<endl;
            }

            if(*vciter1==*vciter){
              aDistance=Distance;
              aESeed=ESeed;
            }

             weightsum+=ESeed*exp(Para.LateralProfile()*Distance/Para.MoliereRadius());
             //weightsum+=ESeed*(1.47*exp(-Distance*2.66/3.8));
          }

           weight=aESeed*exp(Para.LateralProfile()*aDistance/Para.MoliereRadius())/weightsum;
          //weight=aESeed*(1.47*exp(-aDistance*2.66/3.8))/weightsum;
          aHit.setWeight(weight*1.0);

          aShower.Insert(mciter2->first,aHit);
        }
        m_ShowerE->Energy(aShower);
        m_ShowerPos->Position(aShower);
        m_ShowerShape->CalculateMoment(aShower);
        TVector3 newShowerPos(aShower.Position());
        TVector3 oldShowerPos(ShowerPos[*vciter]);
        PosChange+=(newShowerPos-oldShowerPos).Mag();
        tmpShowerMap[aShower.SeedID()]=aShower;
        ShowerPos[*vciter]=newShowerPos;
      }

      PosChange/=(double)SeedIDVec.size();
      for(mciter1=ShowerPos.begin();mciter1!=ShowerPos.end();mciter1++){
        ShowerPos[mciter1->first]=tmpShowerMap[mciter1->first].Position();
      }

      iterations++;


    }while((iterations<8)&&(PosChange>0.01));
    
    for(mciter3=tmpShowerMap.begin();mciter3!=tmpShowerMap.end();mciter3++){
      ShowerMap[mciter3->first]=mciter3->second;
    }

    tmpShowerMap.clear();
  }
}

void ClusterSplitter::Splitmulti(Cluster& aCluster,const vector<int>& SeedIDVec,multimap<int,Shower>& ShowerMap)
{
  Parameter& Para=Parameter::GetInstance();

  map<int,RecHit>::const_iterator citer;

  if(SeedIDVec.size()==0){
    return;
  }
  else if(SeedIDVec.size()==1){
    Shower aShower;
    aShower.Clear();
    aShower.setSeedID(SeedIDVec[0]);
    aShower.setClusterID(aCluster.GetClusterID());

    for(citer=aCluster.Begin();citer!=aCluster.End();citer++){
      aShower.Insert(citer->first,citer->second);
    }

    m_ShowerE->Energy(aShower);
    m_ShowerPos->Position(aShower);
    m_ShowerShape->CalculateMoment(aShower);

    ShowerMap.insert({SeedIDVec[0], aShower});
  }
  else{
    unsigned int iterations=0; //Number of iterations
    double PosChange; //Change in position between two iterations

    map<int,TVector3> ShowerPos;
    map<int,TVector3>::const_iterator mciter1;
    map<int,RecHit>::const_iterator mciter2;

    vector<int>::const_iterator vciter;
    vector<int>::const_iterator vciter1;

    for(vciter=SeedIDVec.begin();vciter!=SeedIDVec.end();vciter++){
      mciter2=aCluster.Find(*vciter);
      if(mciter2!=aCluster.End())
      ShowerPos[mciter2->first]=mciter2->second.FrontCenter();
    }
    
    map<int,Shower> tmpShowerMap;
    map<int,Shower>::const_iterator mciter3;
    
    do{
      PosChange=0;
      tmpShowerMap.clear();

      for(vciter=SeedIDVec.begin();vciter!=SeedIDVec.end();vciter++){
        Shower aShower;
        aShower.Clear();
        aShower.setSeedID(*vciter);
        aShower.setClusterID(aCluster.GetClusterID());

        //calculate the weight for each cell for each seed
        for(mciter2=aCluster.Begin();mciter2!=aCluster.End();mciter2++){
          RecHit aHit=mciter2->second;
          double aDistance=0;
          double aESeed=0;
          //bool isSeed=false;
        
          double weightsum=0,weight=0;

          for(vciter1=SeedIDVec.begin();vciter1!=SeedIDVec.end();vciter1++){
            TVector3 ShowerPoint(ShowerPos[*vciter1]);
            TVector3 HitPoint(aHit.FrontCenter());

            double ESeed=aCluster.Find(*vciter1)->second.Energy();
            double Distance;

            if(*vciter1==mciter2->first){
              //isSeed=true;
              Distance=0;
            }
            else{
              double t=(HitPoint.x()*ShowerPoint.x()+HitPoint.y()*ShowerPoint.y()+HitPoint.z()*ShowerPoint.z())/(ShowerPoint.x()*ShowerPoint.x()+ShowerPoint.y()*ShowerPoint.y()+ShowerPoint.z()*ShowerPoint.z());
              Distance=(HitPoint-t*ShowerPoint).Mag();
            }

            if(*vciter1==*vciter){
              aDistance=Distance;
              aESeed=ESeed;
            }

            weightsum+=ESeed*exp(Para.LateralProfile()*Distance/Para.MoliereRadius());
          }

          weight=aESeed*exp(Para.LateralProfile()*aDistance/Para.MoliereRadius())/weightsum;

          //if(aHit.Energy()*aHit.Weight()>Para.ElecNoiseLevel()){
          aHit.setWeight(weight*1.0);
          aShower.Insert(mciter2->first,aHit);
        }
        m_ShowerE->Energy(aShower);
        m_ShowerPos->Position(aShower);
        m_ShowerShape->CalculateMoment(aShower);
        TVector3 newShowerPos(aShower.Position());
        TVector3 oldShowerPos(ShowerPos[*vciter]);
        PosChange+=(newShowerPos-oldShowerPos).Mag();
        tmpShowerMap.insert({aShower.SeedID(),aShower});
        ShowerPos[*vciter]=newShowerPos;
      }

      PosChange/=(double)SeedIDVec.size();
      for(mciter1=ShowerPos.begin();mciter1!=ShowerPos.end();mciter1++){
        ShowerPos[mciter1->first]=tmpShowerMap[mciter1->first].Position();
      }

      iterations++;

    }while((iterations<8)&&(PosChange>0.01));

    for(mciter3=tmpShowerMap.begin();mciter3!=tmpShowerMap.end();mciter3++){
      ShowerMap.insert({mciter3->first, mciter3->second});
    }

    tmpShowerMap.clear();
  }
}
