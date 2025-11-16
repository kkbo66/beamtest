#ifndef RECHIT_HH
#define RECHIT_HH

#include <TVector3.h>
#include <TRandom3.h>

using namespace std;

class RecHit
{
public:
  // Constructors and Destructors
  RecHit();
  ~RecHit();

  void Clear();
  int CrystalID() const;
  double Energy() const;
  double Weight() const;
  double Time() const;
  TVector3 FrontCenter() const;
  void setCrystalID(const int id);
  void setEnergy(const double amp, const double hgnoise, const double hgpedestal, const double hgmip);
  void setEnergy(const double hgamp, const double lgamp, const double ratio, const double hgsatupoint, const double hgnoise, const double lgnoise, const double hgmip, const double hgpedestal);
  void setEnergy(const double hgpeak, const double lgpeak, const double ratio, const double hgsatupoint, const double hgpedestal, const double hgnoise, const double lgpedestal, const double lgnoise, const double lgmip, const double hgmip, TRandom3 r1);
  void setEnergy(const double hgpeak, const double lgpeak, const double ratio, const double hgsatupoint, const double hgnoise, const double lgpedestal, const double lgnoise, const double lgmip, const double hgmip);
  void setEnergy(const double peak, const double lgpedestal, const bool isHG, const double hgnoise, const double lgnoise, const double ratio, const double lgmip);
  void setWeight(const double w);
  void setTime(const double t);
  void setTime(const double hgtime, const double lgtime, const double hgpeak, const double hgpedestal, const double hgsatupoint);
  void setFrontCenter(const TVector3 FC);

private:
  int m_CrystalID;
  double m_Energy;
  double m_Weight;
  double m_Time;
  TVector3 m_FrontCenter;
};
#endif
