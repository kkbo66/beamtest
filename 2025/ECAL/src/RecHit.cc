#include <iostream>
#include "RecHit.hh"

//Constructors and Destructors
RecHit::RecHit()
{
  Clear();
}

RecHit::~RecHit()
{
  
}

void RecHit::Clear()
{
  m_CrystalID=0;
  m_Energy=0;
  m_Weight=1.0;
  m_Time=0;
  m_FrontCenter.SetXYZ(99.0,99.0,99.0);
}

int RecHit::CrystalID() const
{
  return m_CrystalID;
}

double RecHit::Energy() const
{
  return m_Energy;
}

double RecHit::Weight() const
{
  return m_Weight;
}

double RecHit::Time() const
{
  return m_Time;
}

TVector3 RecHit::FrontCenter() const
{
  return m_FrontCenter;
}

void RecHit::setCrystalID(const int id)
{
  m_CrystalID=id;
}

void RecHit::setEnergy(const double hgpeak,const double lgpeak,const double ratio,const double hgsatupoint,const double hgpedestal,const double hgnoise,const double lgpedestal,const double lgnoise,const double lgmip,TRandom3 r1)
{
  if((hgpeak-hgpedestal)<(hgsatupoint-400)){
    m_Energy=hgpeak-hgpedestal;
    if(m_Energy<3*hgnoise) m_Energy=0;
    else m_Energy=(m_Energy/ratio)/(lgmip-lgpedestal)*168;
    //else m_Energy=(m_Energy/ratio)/(lgmip-lgpedestal)*r1.Gaus(168,3);
  }
  else{
    m_Energy=lgpeak-lgpedestal;
    if(m_Energy<3*lgnoise) m_Energy=0;
    else m_Energy=m_Energy/(lgmip-lgpedestal)*168;
    //else m_Energy=m_Energy/(lgmip-lgpedestal)*r1.Gaus(168,3);
  }

    /*m_Energy=lgpeak-lgpedestal;
    if(m_Energy<3*lgnoise) m_Energy=0;
    else m_Energy=m_Energy/(lgmip-lgpedestal)*168;*/
    //else m_Energy=m_Energy/(lgmip-lgpedestal)*r1.Gaus(168,3);

}

void RecHit::setEnergy(const double hgpeak,const double lgpeak,const double ratio,const double hgsatupoint,const double hgnoise,const double lgpedestal,const double lgnoise,const double lgmip)
{
  if((hgpeak)<(hgsatupoint-400)){
    m_Energy=hgpeak;
    if(m_Energy<3*hgnoise) m_Energy=0;
    else m_Energy=(m_Energy/ratio)/(lgmip-lgpedestal)*168;
  }
  else{
    m_Energy=lgpeak;
    if(m_Energy<3*lgnoise) m_Energy=0;
    else m_Energy=m_Energy/(lgmip-lgpedestal)*168;
  }


}


void RecHit::setEnergy(const double peak, const double lgpedestal,const bool isHG,const double hgnoise,const double lgnoise,const double ratio,const double lgmip)
{
  if(isHG==true){
    m_Energy=peak;
    if(m_Energy<3*hgnoise) m_Energy=0;
    else m_Energy=(m_Energy/ratio)/(lgmip-lgpedestal)*168;
  }
  else{
    m_Energy=peak;
    if(m_Energy<3*lgnoise) m_Energy=0;
    else m_Energy=m_Energy/(lgmip-lgpedestal)*168;
  }
}

void RecHit::setWeight(const double w)
{
  m_Weight=w;
}
  
void RecHit::setTime(const double t)
{
  m_Time=t;
}

void RecHit::setTime(const double hgtime,const double lgtime,const double hgpeak,const double hgpedestal,const double hgsatupoint)
{
  if((hgpeak-hgpedestal)<(hgsatupoint-400)) m_Time=hgtime;
  else m_Time=lgtime;
}

void RecHit::setFrontCenter(const TVector3 FC)
{
 m_FrontCenter=FC;
}
