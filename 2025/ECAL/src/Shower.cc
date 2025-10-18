#include <iostream>

#include "Shower.hh"

//Constructors and Destructors

Shower::Shower()
{
  Clear();
}

Shower::~Shower()
{
  
}

map<int,RecHit> Shower::HitWithWeightMap() const
{
  map<int,RecHit> tmpMap;
  map<int,RecHit>::const_iterator citer;

  for(citer=m_HitWithWeightMap.begin();citer!=m_HitWithWeightMap.end();citer++){
    tmpMap[citer->first]=citer->second;
  }

  return tmpMap;
}

map<int,RecHit> Shower::HitWithWeightMap3x3() const
{
  
  map<int,RecHit> tmpMap;
  map<int,RecHit>::const_iterator citer;
  if(m_HitWithWeightMap.empty()) return tmpMap;

  vector<int>::const_iterator iter;

  for(citer=m_HitWithWeightMap.begin();citer!=m_HitWithWeightMap.end();citer++){
    for(iter=m_ID3x3.begin();iter!=m_ID3x3.end();iter++){
      if(citer->first==*iter){
         tmpMap[citer->first]=citer->second;
      }
    }
  }

  return tmpMap;
}

map<int,RecHit> Shower::HitWithWeightMap5x5() const
{  
  map<int,RecHit> tmpMap;
  map<int,RecHit>::const_iterator citer;
  if(m_HitWithWeightMap.empty()) return tmpMap;

  vector<int>::const_iterator iter;

  for(citer=m_HitWithWeightMap.begin();citer!=m_HitWithWeightMap.end();citer++){
    for(iter=m_ID5x5.begin();iter!=m_ID5x5.end();iter++){
      if(citer->first==*iter){
         tmpMap[citer->first]=citer->second;
      }
    }
  }

  return tmpMap;
}

map<int,RecHit>::const_iterator Shower::Begin() const
{
  return m_HitWithWeightMap.begin();
}

map<int,RecHit>::const_iterator Shower::End() const
{
  return m_HitWithWeightMap.end();
}

map<int,RecHit>::const_iterator Shower::Find(const int ID) const
{
  //if failed then return end()
  return m_HitWithWeightMap.find(ID);
}

void Shower::Insert(const int ID, const RecHit hit)
{
  m_HitWithWeightMap[ID]=hit;
}

void Shower::Erase(const int ID)
{
  map<int,RecHit>::const_iterator citer;
  citer=m_HitWithWeightMap.find(ID);

  if(m_HitWithWeightMap.empty()){
    return;
  }

  if(citer==End()){
    return;
  }

  if(citer!=End()){
    m_HitWithWeightMap.erase(citer->first);
    
    if(m_HitWithWeightMap.empty()){
        Clear();
        return;
    }
    else{
      return;
    }
  }
}

void Shower::Clear()
{
  m_SeedID=0;
  m_ClusterID=0;
  m_numHits=0;
  m_position.SetXYZ(99.0,99.0,99.0);
  m_position3x3.SetXYZ(99.0,99.0,99.0);
  m_position5x5.SetXYZ(99.0,99.0,99.0);
  m_Energy=0;
  m_ESeed=0;
  m_E3x3=0;
  m_E5x5=0;
  m_ID3x3.clear();
  m_ID5x5.clear();
  m_HitWithWeightMap.clear();
}
