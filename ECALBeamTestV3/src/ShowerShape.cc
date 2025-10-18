#include <iostream>
#include <iomanip>

#include "ShowerShape.hh"
#include "RecHit.hh"

#include <complex>

//Constructors and Destructors
ShowerShape::ShowerShape()
{

}

ShowerShape::~ShowerShape()
{

}

void ShowerShape::CalculateMoment(Shower& aShower) const
{
 
  SecondMoment(aShower);
  LatMoment(aShower);
  A20Moment(aShower);
  A42Moment(aShower);
}

void ShowerShape::SecondMoment(Shower& aShower) const
{
  double etot=0;
  double sum=0;
  TVector3 center=TVector3(aShower.Position());

  map<int,RecHit> HitMap5x5(aShower.HitWithWeightMap5x5());

  map<int,RecHit>::const_iterator citer;
  for(citer=HitMap5x5.begin();citer!=HitMap5x5.end();citer++){
    TVector3 pos=TVector3(citer->second.FrontCenter());
    double weight =citer->second.Weight();
    etot+=citer->second.Energy()*weight;
    sum+=citer->second.Energy()*weight*((pos-center).Mag2());
  }

  if(etot>0) sum/=etot;
  double epsilon =1e-5;
  double dsum=std::abs(sum-(std::floor(sum * 100000) / 100000));
  if(sum<epsilon) aShower.setSecondMoment(std::floor(sum * 100000) / 100000);
  else aShower.setSecondMoment(sum);
}

void ShowerShape::LatMoment(Shower& aShower) const
{
  map<int,RecHit> HitMap5x5(aShower.HitWithWeightMap5x5());
  map<int,RecHit>::const_iterator citer;
  vector<double> Ene;
  vector<TVector3> Pos;

  for(citer=HitMap5x5.begin();citer!=HitMap5x5.end();citer++){
    TVector3 pos=citer->second.FrontCenter();
    double weight = citer->second.Weight();
    Ene.push_back(citer->second.Energy()*weight);
    Pos.push_back(pos);
  }

  if(Ene.size()<2){
    aShower.setLateralMoment(0);
    return;
  }

  //Find the largest 2 Hit
  int MaxID=999;
  int SecondID=999;
  double MaxE=0;
  double SecondE=0;

  for(int i=0;i<Ene.size();i++){
    if(Ene.at(i)>MaxE){
      MaxID=i;
      MaxE=Ene.at(i);
    }
  }
  
  for(int i=0;i<Ene.size();i++){
    if(i!=MaxID&&Ene.at(i)>SecondE){
      SecondID=i;
      SecondE=Ene.at(i);
    }
  }
  

  //Calculate LATMoment
  double numerator=0;
  double denominator=0;
  TVector3 center=TVector3(aShower.Position());
  for(int i=0;i<Ene.size();i++){
    double r=Pos.at(i).Mag()*TMath::Sin(center.Angle(Pos.at(i)));
    if(i==MaxID||i==SecondID) denominator+=5*5*Ene.at(i);
    else{
      numerator+=r*r*Ene.at(i);
      denominator+=r*r*Ene.at(i);
    }
  }

  double latmoment=-999;
  if(denominator>0) latmoment=numerator/denominator;

  aShower.setLateralMoment(latmoment);
}


void ShowerShape::A20Moment(Shower& aShower) const
{
  double A20=0;
  const double R=15.0;

  TVector3 center=TVector3(aShower.Position());

  map<int,RecHit> HitMap5x5(aShower.HitWithWeightMap5x5());
  map<int,RecHit>::const_iterator citer;
  for(citer=HitMap5x5.begin();citer!=HitMap5x5.end();citer++){
    TVector3 pos=TVector3(citer->second.FrontCenter());
    double weight = citer->second.Weight();
    double energy=citer->second.Energy()*weight;
 
    TVector3 r=pos-center;
    r=r-r.Dot(center)*center*(1.0/center.Mag2());

    A20+=(energy/aShower.E5x5())*(2*TMath::Power(r.Mag()/R,2)-1);
  }

  double epsilon =1e-4;
  double dA20=std::abs(A20-(std::floor(A20 * 100000) / 100000));
  if(dA20<epsilon) aShower.setA20Moment(abs(std::floor(A20 * 100000) / 100000));
  else aShower.setA20Moment(abs(A20));
}
void ShowerShape::A42Moment(Shower& aShower) const
{
  complex<double> A42=(0,0);
  const double R=15.0;

  TVector3 center=TVector3(aShower.Position());

  map<int,RecHit> HitMap5x5(aShower.HitWithWeightMap5x5());
  map<int,RecHit>::const_iterator citer;
  for(citer=HitMap5x5.begin();citer!=HitMap5x5.end();citer++){
    TVector3 pos=TVector3(citer->second.FrontCenter());
    double weight = citer->second.Weight();;
    double energy=citer->second.Energy()*weight;

    TVector3 r=pos-center;
    r=r-r.Dot(center)*center*(1.0/center.Mag2());

    complex<double> a(0,2*r.Phi());

    A42+=(energy/aShower.E5x5())*(4*TMath::Power(r.Mag()/R,4)-3*TMath::Power(r.Mag()/R,2))*exp(a);
  }
  
  //cout<<"gg: "<<std::fixed<<std::setprecision(100)<<abs(A42)<<endl;
  //cout<<std::abs(abs(A42)-(std::floor(abs(A42) * 100000) / 100000))<<endl;
  double epsilon =1e-10;
  double dA42=std::abs(abs(A42)-(std::floor(abs(A42) * 100000) / 100000));
  if(dA42<epsilon) aShower.setA42Moment(abs(std::floor(abs(A42) * 100000) / 100000));
  else aShower.setA42Moment(abs(A42));
}
