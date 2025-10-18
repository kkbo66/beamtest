#include <iostream>

#include "Cluster.hh"

//Constructors and Destructors
Cluster::Cluster()
{
  Clear();
}

Cluster::~Cluster()
{
  Clear();
}

void Cluster::Clear()
{
  m_ClusterID=0;
  m_HitMap.clear();
  Energy=0;
  SecondMoment=0;
  Position.SetXYZ(99.0,99.0,99.0); 
}

void Cluster::setClusterID(const int ID)
{
  m_ClusterID=ID;
}

void Cluster::setEnergy(const double e)
{
  Energy=e;
}

void Cluster::setSecondMoment (const double sm)
{
  SecondMoment=sm;
}

void Cluster::setPosition (const TVector3 pos)
{
  Position=pos;
}

int Cluster::GetClusterSize() const
{
  return m_HitMap.size();
}

map<int,RecHit>::const_iterator Cluster::Begin() const
{
  return m_HitMap.begin();
}

map<int,RecHit>::const_iterator Cluster::End() const
{
  return m_HitMap.end();
}

map<int,RecHit>::const_iterator Cluster::Find(const int ID) const
{
  return m_HitMap.find(ID);
}

void Cluster::Insert(const int ID, const RecHit hit)
{
  m_HitMap[ID]=hit;
}

void Cluster::Erase(const int ID)
{
  map<int,RecHit>::const_iterator iter;
  iter=m_HitMap.find(ID);
  if(iter!=End()){
    m_HitMap.erase(ID);
  }
}

map<int,RecHit>::const_iterator Cluster::SeedBegin() const
{
  return m_SeedMap.begin();
}

map<int,RecHit>::const_iterator Cluster::SeedEnd() const
{
  return m_SeedMap.end();
}

map<int,RecHit>::const_iterator Cluster::SeedFind(const int ID) const
{
  return m_SeedMap.find(ID);
}

void Cluster::InsertSeed(const int ID,const RecHit SeedHit)
{
  m_SeedMap[ID]=SeedHit;
}

int Cluster::SeedSize() const
{
  return m_SeedMap.size();
}
