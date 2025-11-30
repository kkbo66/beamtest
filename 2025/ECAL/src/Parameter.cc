#include "Parameter.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string>

// Initialize static data member
Parameter *Parameter::m_ParaInstance = 0;
// Get path of executable program
std::string getExecutablePath()
{
  char buf[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  if (len != -1)
  {
    buf[len] = '\0';
    return std::string(buf);
  }
  return "";
}
// Constructors and Destructors
Parameter::Parameter()
{
  string ParaPath = getExecutablePath();
  size_t lastSlashPos = ParaPath.find_last_of('/');
  if (lastSlashPos != std::string::npos)
  {
    ParaPath = ParaPath.substr(0, lastSlashPos);
  }
  lastSlashPos = ParaPath.find_last_of('/');
  if (lastSlashPos != std::string::npos)
  {
    ParaPath = ParaPath.substr(0, lastSlashPos);
  }

  ParaPath += "/parameter/";
  string ParaPath1(ParaPath);
  string ParaPath2(ParaPath);
  string ParaPath3(ParaPath);
  string ParaPath4(ParaPath);
  string ParaPath5(ParaPath);
  string ParaPath6(ParaPath);
  string ParaPath7(ParaPath);

  ParaPath1 += "CommomPara.dat";
  ifstream in1;
  in1.open(ParaPath1);
  if (!in1.good())
  {
    cout << "Error in Opening File" << endl;
  }
  assert(in1);
  const int MaxCharOfOneLine = 255;
  char temp[MaxCharOfOneLine];
  int InputNo = 0;
  while (in1.peek() != EOF)
  {
    in1.getline(temp, MaxCharOfOneLine);
    if (temp[0] == '#')
      continue;
    InputNo++;
    switch (InputNo)
    {
    case 1:
      istringstream(temp) >> m_EThresholdSeed >> m_EThresholdCluster;
      break;
    case 2:
      istringstream(temp) >> m_LogPosOffset >> m_PositionMode1 >> m_PositionMode2;
      break;
    case 3:
      istringstream(temp) >> m_LateralProfile >> m_MoliereRadius;
      break;
    default:
      break;
    }
  }
  in1.close();

  ParaPath2 += "HitPos.dat";
  ifstream in2;
  in2.open(ParaPath2);
  if (!in2.good())
  {
    cout << "Error in Opening File" << endl;
  }
  assert(in2);

  int crystalid;
  double hitposx, hitposy, hitposz;
  for (int i = 0; i < 25; i++)
  {
    in2 >> crystalid >> hitposz >> hitposy >> hitposx;
    m_HitPos[crystalid] = TVector3(hitposx, hitposy, hitposz + 105);
  }
  in2.close();

  ParaPath3 += "CaliPara.dat";
  ifstream in3;
  in3.open(ParaPath3);
  if (!in3.good())
  {
    cout << "Error in Opening File" << endl;
  }
  assert(in3);

  // double DACRatio;
  // double RayRatio;
  for (int i = 0; i < 25; i++)
  {
    // in3>>m_HGCali[i]>>m_LGCali[i]>>DACRatio>>RayRatio>>m_HGSatuPoint[i];
    in3 >> m_LGPedestal[i] >> m_LGNoise[i] >> m_HGPedestal[i] >> m_HGNoise[i] >> m_Ratio[i] >> m_HGSatuPoint[i];
  }
  in3.close();

  /*ParaPath4+="Pedestal.dat";
  ifstream in4;
  in4.open(ParaPath4);
  if(!in4.good()){
    cout<<"Error in Opening File"<<endl;
  }
  assert(in4);

  double LGNoisefC;
  double HGNoisefC;
  for(int i=0;i<25;i++){
    in4>>m_LGPedestal[i]>>m_LGNoise[i]>>LGNoisefC>>m_HGPedestal[i]>>m_HGNoise[i]>>HGNoisefC;
  }
  in4.close();*/

  ParaPath5 += "LGMipPeak.dat";
  ifstream in5;
  in5.open(ParaPath5);
  if (!in5.good())
  {
    cout << "Error in Opening File" << endl;
  }
  assert(in5);

  double LGPeak;
  for (int i = 0; i < 25; i++)
  {
    // in5>>LGPeak>>m_LGLightYield[i]>>HGPeak>>m_HGLightYield[i];
    in5 >> m_LGMipPeak[i];
  }
  in5.close();

  ParaPath6 += "HGMipPeak.dat";
  ifstream in6;
  in6.open(ParaPath6);
  if (!in6.good())
  {
    cout << "Error in Opening File" << endl;
  }
  assert(in6);

  double HGPeak;
  for (int i = 0; i < 25; i++)
  {
    // in6>>LGPeak>>m_LGLightYield[i]>>HGPeak>>m_HGLightYield[i];
    in6 >> m_HGMipPeak[i];
  }
  in6.close();

  ParaPath7 += "WaveformPara.dat";
  ifstream in7;
  in7.open(ParaPath7);
  if (!in7.good())
  {
    cout << "Error in Opening File" << endl;
  }
  assert(in7);

  for (int i = 0; i < 50; i++)
  {
    if (i % 2 == 0)
    {
      for (int j = 0; j < 9; j++)
        in7 >> m_LGWfPara[i / 2][j];
    }
    else
    {
      for (int j = 0; j < 9; j++)
        in7 >> m_HGWfPara[i / 2][j];
    }
  }
  in7.close();
}

Parameter::~Parameter()
{
}

// static method
Parameter &Parameter::GetInstance()
{
  if (!Exist())
  {
    m_ParaInstance = new Parameter();
  }
  return *m_ParaInstance;
}

bool Parameter::Exist()
{
  return m_ParaInstance != 0;
}

void Parameter::Kill()
{
  if (Exist())
  {
    delete m_ParaInstance;
    m_ParaInstance = 0;
  }
}

double Parameter::EThresholdSeed() const
{
  return m_EThresholdSeed;
}

double Parameter::EThresholdCluster() const
{
  return m_EThresholdCluster;
}

double Parameter::LogPosOffset() const
{
  return m_LogPosOffset;
}

double Parameter::LateralProfile() const
{
  return m_LateralProfile;
}

double Parameter::MoliereRadius() const
{
  return m_MoliereRadius;
}

string Parameter::PositionMode1() const
{
  return m_PositionMode1;
}

string Parameter::PositionMode2() const
{
  return m_PositionMode2;
}

double Parameter::HitPosX(int n) const
{
  auto it = m_HitPos.find(n);
  if (it != m_HitPos.end())
    return it->second.x();
  else
    return 99;
}

double Parameter::HitPosY(int n) const
{
  auto it = m_HitPos.find(n);
  if (it != m_HitPos.end())
    return it->second.y();
  else
    return 99;
}

double Parameter::HitPosZ(int n) const
{
  auto it = m_HitPos.find(n);
  if (it != m_HitPos.end())
    return it->second.z();
  else
    return 99;
}

double Parameter::HGCali(int n) const
{
  return m_HGCali[n];
}

double Parameter::LGCali(int n) const
{
  return m_LGCali[n];
}

double Parameter::Ratio(int n) const
{
  return m_Ratio[n];
}

double Parameter::HGSatuPoint(int n) const
{
  return m_HGSatuPoint[n];
}

double Parameter::HGPedestal(int n) const
{
  return m_HGPedestal[n];
}

double Parameter::HGNoise(int n) const
{
  return m_HGNoise[n];
}

double Parameter::LGPedestal(int n) const
{
  return m_LGPedestal[n];
}

double Parameter::LGNoise(int n) const
{
  return m_LGNoise[n];
}

double Parameter::HGLightYield(int n) const
{
  return m_HGLightYield[n];
}

double Parameter::LGLightYield(int n) const
{
  return m_LGLightYield[n];
}

double Parameter::LGMipPeak(int n) const
{
  return m_LGMipPeak[n];
}

double Parameter::HGMipPeak(int n) const
{
  return m_HGMipPeak[n];
}

double Parameter::HGWfPara(int m, int n) const
{
  return m_HGWfPara[m][n];
}

double Parameter::LGWfPara(int m, int n) const
{
  return m_LGWfPara[m][n];
}
