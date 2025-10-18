#ifndef CLUSTER_HH
#define CLUSTER_HH

#include <map>
#include "data_model.hh"
#include "RecHit.hh"
#include <TVector3.h>

using namespace std;

class Cluster
{
  public:
  //Constructors and Destructors
  Cluster();
  ~Cluster();

  void Clear();
  void setClusterID (const int ID);
  void setEnergy (const double e);
  void setSecondMoment (const double sm);
  void setPosition (const TVector3 pos);

  int GetClusterID() const { return m_ClusterID;}
  double GetClusterEnergy() const { return Energy;}
  double GetSecondMoment() const { return SecondMoment;}
  TVector3 GetPosition() const { return Position;}
  int GetClusterSize() const;

  map<int,RecHit>::const_iterator Begin() const;
  map<int,RecHit>::const_iterator End() const;
  map<int,RecHit>::const_iterator Find(const int ID) const;

  void Insert(const int ID, const RecHit hit);
  void Erase(const int ID);

  map<int,RecHit>::const_iterator SeedBegin() const;
  map<int,RecHit>::const_iterator SeedEnd() const;
  map<int,RecHit>::const_iterator SeedFind(const int ID) const;

  void InsertSeed(const int ID,const RecHit SeedHit);
  int SeedSize() const;


  protected:
  int m_ClusterID;
  double Energy;
  double SecondMoment;
  TVector3 Position;

  map<int,RecHit> m_HitMap;
  map<int,RecHit> m_SeedMap;
};

#endif

