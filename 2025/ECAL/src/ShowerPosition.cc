#include <iostream>
#include <iomanip>

#include "ShowerPosition.hh"
#include "Parameter.hh"
#include "RecHit.hh"

//Constructors and Destructors
ShowerPosition::ShowerPosition()
{

}

ShowerPosition::~ShowerPosition()
{

}

void ShowerPosition::Position(Shower& aShower)
{
  map<int,RecHit>::const_iterator citer;
  TVector3 pos;
  TVector3 possum;

  TVector3 pos3x3;
  TVector3 possum3x3;

  TVector3 pos5x5;
  TVector3 possum5x5;

  Parameter& Para=Parameter::GetInstance();
  double offset=Para.LogPosOffset();

  if(Para.PositionMode1()=="log"){
    double w,wsum=0;
    double w3x3,wsum3x3=0;
    double w5x5,wsum5x5=0;
    if(Para.PositionMode2()=="all"){
      double ETotal=aShower.EAll();
      for(citer=aShower.Begin();citer!=aShower.End();citer++){
        w=offset+log((citer->second.Energy())*(citer->second.Weight())/ETotal);
        if(w>0){
          wsum+=w;
          pos=citer->second.FrontCenter();
          possum+=pos*w;
        }
      }

      double E5x5=aShower.E5x5();
      map<int,RecHit> HitWithWeightMap5x5=aShower.HitWithWeightMap5x5();
      for(citer=HitWithWeightMap5x5.begin();citer!=HitWithWeightMap5x5.end();citer++){
        w5x5=offset+log((citer->second.Energy())*(citer->second.Weight())/E5x5);
        if(w5x5>0){
          wsum5x5+=w5x5;
          pos5x5=citer->second.FrontCenter();
          possum5x5+=pos5x5*w5x5;
        }
      }

      double E3x3=aShower.E3x3();
      map<int,RecHit> HitWithWeightMap3x3=aShower.HitWithWeightMap3x3();
      for(citer=HitWithWeightMap3x3.begin();citer!=HitWithWeightMap3x3.end();citer++){
        w3x3=offset+log((citer->second.Energy())*(citer->second.Weight())/E3x3);
        if(w3x3>0){
          wsum3x3+=w3x3;
          pos3x3=citer->second.FrontCenter();
          possum3x3+=pos3x3*w3x3;
        }
      }
    }
    else if(Para.PositionMode2()=="3x3"){
      double E3x3=aShower.E3x3();
      map<int,RecHit> HitWithWeightMap3x3=aShower.HitWithWeightMap3x3();
      for(citer=HitWithWeightMap3x3.begin();citer!=HitWithWeightMap3x3.end();citer++){
        w=offset+log((citer->second.Energy())*(citer->second.Weight())/E3x3);
        if(w>0){
          wsum+=w;
          pos=citer->second.FrontCenter();
          possum+=pos*w;
        }
      }

      double E5x5=aShower.E5x5();
      map<int,RecHit> HitWithWeightMap5x5=aShower.HitWithWeightMap5x5();
      for(citer=HitWithWeightMap5x5.begin();citer!=HitWithWeightMap5x5.end();citer++){
        w5x5=offset+log((citer->second.Energy())*(citer->second.Weight())/E5x5);
        if(w5x5>0){
          wsum5x5+=w5x5;
          pos5x5=citer->second.FrontCenter();
          possum5x5+=pos5x5*w5x5;
        }
      }
    }
    else {
      double E5x5=aShower.E5x5();
      map<int,RecHit> HitWithWeightMap5x5=aShower.HitWithWeightMap5x5();
      for(citer=HitWithWeightMap5x5.begin();citer!=HitWithWeightMap5x5.end();citer++){
        w=offset+log((citer->second.Energy())*(citer->second.Weight())/E5x5);
        if(w>0){
          wsum+=w;
          pos=citer->second.FrontCenter();
          possum+=pos*w;
        }
      }

      double E3x3=aShower.E3x3();
      map<int,RecHit> HitWithWeightMap3x3=aShower.HitWithWeightMap3x3();
      for(citer=HitWithWeightMap3x3.begin();citer!=HitWithWeightMap3x3.end();citer++){
        w3x3=offset+log((citer->second.Energy())*(citer->second.Weight())/E3x3);
        if(w3x3>0){
          wsum3x3+=w3x3;
          pos3x3=citer->second.FrontCenter();
          possum3x3+=pos3x3*w3x3;
        }
      }
    }

    if(Para.PositionMode2()=="all"){
      if(wsum<=0){
        cout<<"-------------wsum:"<<wsum<<"<=0--------------"<<endl;
      } 
      else{
        pos=possum*(1.0/wsum);
        aShower.setPosition(pos);
      }

      if(wsum3x3<=0){
        cout<<"-------------wsum3x3:"<<wsum3x3<<"<=0--------------"<<endl;
      } 
      else{
        pos3x3=possum3x3*(1.0/wsum3x3);
        aShower.setPosition3x3(pos3x3);
      }

      if(wsum5x5<=0){
        cout<<"-------------wsum5x5:"<<wsum5x5<<"<=0--------------"<<endl;
      } 
      else{
        pos5x5=possum5x5*(1.0/wsum5x5);
        aShower.setPosition5x5(pos5x5);
      }
    }
    
    else if(Para.PositionMode2()=="3x3"){
      if(wsum<=0){
        cout<<"-------------wsum:"<<wsum<<"<=0--------------"<<endl;
      } 
      else{
        pos=possum*(1.0/wsum);
        aShower.setPosition(pos);
        aShower.setPosition3x3(pos);
      }

      if(wsum5x5<=0){
        cout<<"-------------wsum5x5:"<<wsum5x5<<"<=0--------------"<<endl;
      } 
      else{
        pos5x5=possum5x5*(1.0/wsum5x5);
        aShower.setPosition5x5(pos5x5);
      }
    }
    
    else{
     if(wsum<=0){
        cout<<"-------------wsum:"<<wsum<<"<=0--------------"<<endl;
      } 
      else{
        pos=possum*(1.0/wsum);
        //cout<<"gg: "<<std::fixed<<std::setprecision(15)<<pos.x()-99<<endl;
        double epsilon =1e-5;
        double dx=std::abs(pos.x()-(std::floor(pos.x() * 100000) / 100000));
        double dy=std::abs(pos.y()-(std::floor(pos.y() * 100000) / 100000));
        double dz=std::abs(pos.z()-(std::floor(pos.z() * 100000) / 100000));
        //cout<<dx<<" "<<dy<<" "<<dz<<endl;
        if(dx<epsilon||dy<epsilon||dz<epsilon){
          aShower.setPosition(TVector3(std::floor(pos.x() * 100000) / 100000,std::floor(pos.y() * 100000) / 100000,std::floor(pos.z() * 100000) / 100000));
        aShower.setPosition5x5(TVector3(std::floor(pos.x() * 100000) / 100000,std::floor(pos.y() * 100000) / 100000,std::floor(pos.z() * 100000) / 100000));
        }
          else {
        aShower.setPosition(pos);
        aShower.setPosition5x5(pos);
          }
      }

      if(wsum3x3<=0){
        cout<<"-------------wsum3x3:"<<wsum3x3<<"<=0--------------"<<endl;
      } 
      else{
        pos3x3=possum3x3*(1.0/wsum3x3);
        double epsilon =1e-5;
        if(std::abs(pos3x3.x()-(std::floor(pos3x3.x() * 100000) / 100000))<epsilon) aShower.setPosition3x3(TVector3(std::floor(pos3x3.x() * 100000) / 100000,std::floor(pos3x3.y() * 100000) / 100000,std::floor(pos3x3.z() * 100000) / 100000));
          else aShower.setPosition3x3(pos3x3);
      }
    }
  }

  if(Para.PositionMode1()=="linear"){
    if(Para.PositionMode2()=="all"){
      double ETotal=aShower.EAll();
      for(citer=aShower.Begin();citer!=aShower.End();citer++){
        pos=citer->second.FrontCenter()*(citer->second.Energy())*(citer->second.Weight());
        possum+=pos*(1.0/ETotal);
      }
    }
    else if(Para.PositionMode2()=="3x3"){
      double E3x3=aShower.E3x3();
      map<int,RecHit> HitWithWeightMap3x3=aShower.HitWithWeightMap3x3();
      for(citer=HitWithWeightMap3x3.begin();citer!=HitWithWeightMap3x3.end();citer++){
        pos=citer->second.FrontCenter()*(citer->second.Energy())*(citer->second.Weight());
        possum+=pos*(1.0/E3x3);  
      }
    }
    else{
      double E5x5=aShower.E5x5();
      map<int,RecHit> HitWithWeightMap5x5=aShower.HitWithWeightMap5x5();
      for(citer=HitWithWeightMap5x5.begin();citer!=HitWithWeightMap5x5.end();citer++){
        pos=citer->second.FrontCenter()*(citer->second.Energy())*(citer->second.Weight());
        possum+=pos*(1.0/E5x5);  
      }
    } 
    aShower.setPosition(possum); 
  }






}
