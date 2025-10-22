#ifndef PARAMETER_HH
#define PARAMETER_HH

#include <string>
#include <map>
#include <TVector3.h>

using namespace std;

class Parameter
{
  private:
    //Constructors and Destructors
    Parameter();
    ~Parameter();

  public:
    //static member functions
    static Parameter& GetInstance();
    static bool Exist();
    static void Kill();

  private:
    //static data members
    static Parameter* m_ParaInstance;

  public:
    //access to each parameter
    double EThresholdSeed() const;
    double EThresholdCluster() const;
    double LogPosOffset() const;
    double LateralProfile() const;
    double MoliereRadius() const;
    string PositionMode1() const;
    string PositionMode2() const;
    double HitPosX(int n) const;
    double HitPosY(int n) const;
    double HitPosZ(int n) const;
    double HGCali(int n) const;
    double LGCali(int n) const;
    double Ratio(int n) const;
    double HGSatuPoint(int n) const;
    double HGPedestal(int n) const;
    double HGNoise(int n) const;
    double LGPedestal(int n) const;
    double LGNoise(int n) const;
    double HGLightYield(int n) const;
    double LGLightYield(int n) const;
    double LGMipPeak(int n) const;
    double HGMipPeak(int n) const;
    double HGWfPara(int m,int n) const;
    double LGWfPara(int m,int n) const;
 
  private:
    //each parameter
    double m_EThresholdSeed; //Energy threshold for seed search
    double m_EThresholdCluster; //Energy threshold for Cluster search
    double m_LogPosOffset; // Log-Weighted Position offset
    double m_LateralProfile; //Lateral profile
    double m_MoliereRadius; //Moliere radius
    string m_PositionMode1;
    string m_PositionMode2;
    map<int,TVector3> m_HitPos;
    double m_HGCali[25];
    double m_LGCali[25];
    double m_Ratio[25];
    double m_HGSatuPoint[25];
    double m_HGPedestal[25];
    double m_HGNoise[25];
    double m_LGPedestal[25];
    double m_LGNoise[25];
    double m_HGLightYield[25];
    double m_LGLightYield[25];
    double m_LGMipPeak[25];
    double m_HGMipPeak[25];
    double m_HGWfPara[25][11];
    double m_LGWfPara[25][11];

 
};
#endif
