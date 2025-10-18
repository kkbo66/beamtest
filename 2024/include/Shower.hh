#ifndef SHOWER_HH
#define SHOWER_HH

#include <iostream>
#include <map>
#include <vector>
#include <TVector3.h>
#include "RecHit.hh"

using namespace std;

class Shower
{
  public:
  //Constructors and Destructors
  Shower();
  ~Shower();

  int SeedID() const {
    return m_SeedID;
  }

  int ClusterID() const{
    return m_ClusterID;
  }

  int NumHits() const{
    return m_numHits;
  }
  
  TVector3 Position() const{
    return m_position;
  }
  
  TVector3 Position3x3() const{
    return m_position3x3;
  }

  TVector3 Position5x5() const{
    return m_position5x5;
  }

  double x() const{
    return m_position.x();
  }

  double y() const{
    return m_position.y();
  }

  double z() const{
    return m_position.z();
  }

  double x3x3() const{
    return m_position3x3.x();
  }

  double y3x3() const{
    return m_position3x3.y();
  }

  double z3x3() const{
    return m_position3x3.z();
  }

  double x5x5() const{
    return m_position5x5.x();
  }

  double y5x5() const{
    return m_position5x5.y();
  }

  double z5x5() const{
    return m_position5x5.z();
  }


  double Energy() const{
    return m_Energy;
  }

  double dE() const{
    return m_dE;
  }


  double ESeed() const{
    return m_ESeed;
  }

  vector<int> ID3x3() const{
    return m_ID3x3;
  }

  vector<int> ID5x5() const{
    return m_ID5x5;
  }

  double E3x3() const{
    return m_E3x3;
  }
  
  double E5x5() const{
    return m_E5x5;
  }

  double EAll() const{
    return m_EAll;
  }

  double SecondMoment() const{
    return m_SecondMoment;
  }
  
  double LateralMoment() const{
    return m_LateralMoment;
  }
  
  double A20Moment() const{
    return m_A20Moment;
  }
  
  double A42Moment() const{
    return m_A42Moment;
  }




  void  setNumHits(int number){
    m_numHits = number;
  }

  void setPosition(const TVector3 pos){
    m_position=pos;
  }

  void setPosition3x3(const TVector3 pos){
    m_position3x3=pos;
  }

  void setPosition5x5(const TVector3 pos){
    m_position5x5=pos;
  }

  void setEnergy(double e){
    m_Energy = e;
  }

  void setDE(double de){
    m_dE=de;
  }

  void setESeed(double eSeed){
    m_ESeed=eSeed;
  }

  void setID3x3(vector<int> IDVec){
    m_ID3x3=IDVec;
  }

  void setID5x5(vector<int> IDVec){
    m_ID5x5=IDVec;
  }

  void setE3x3(double e3x3){
    m_E3x3 = e3x3;
  }

  void setE5x5(double e5x5){
    m_E5x5 = e5x5;
  }

  void setEAll(double eall){
    m_EAll = eall;
  }
  
  void setSeedID(int ID){
    m_SeedID = ID;
  }

  void setSecondMoment(double sm){
    m_SecondMoment=sm;
  }
  
  void setLateralMoment(double lm){
    m_LateralMoment=lm;
  }
  
  void setA20Moment(double a20m){
    m_A20Moment=a20m;
  }
  
  void setA42Moment(double a42m){
    m_A42Moment=a42m;
  }

  void setClusterID(int ID){
    m_ClusterID=ID;
  }

  unsigned int ShowerSize() const{
    return m_HitWithWeightMap.size();
  }
  
  map<int,RecHit> HitWithWeightMap() const;
  map<int,RecHit> HitWithWeightMap3x3() const;
  map<int,RecHit> HitWithWeightMap5x5() const;

  map<int,RecHit>::const_iterator Begin() const;
  map<int,RecHit>::const_iterator End() const;
  map<int,RecHit>::const_iterator Find(const int ID) const;

  

  void Insert(const int ID, const RecHit hit);
  void Erase(const int ID);

  void Clear();

  private:
  int m_SeedID; //ID of Seed
  int m_ClusterID; //ID of Cluster
  int m_numHits; //Number of hit
  TVector3 m_position; //Shower position
  TVector3 m_position3x3; //Shower position 3x3
  TVector3 m_position5x5; //Shower position 5x5
  double m_Energy; //Shower energy after correction
  double m_dE; //Energy Error
  double m_ESeed; //Energy of seed
  vector<int> m_ID3x3; //ID vector,totally 9 ones
  vector<int> m_ID5x5; //ID vector,totally 25 ones
  double m_E3x3; //Energy of 3x3 crystals, totally 9 ones
  double m_E5x5; //Energy of 5x5 crystals, totally 25 ones
  double m_EAll; //Energy with all crystals included
  double m_SecondMoment;
  double m_LateralMoment;
  double m_A20Moment;
  double m_A42Moment;
  map<int,RecHit> m_HitWithWeightMap; // Hits with weight of a shower
};
#endif
