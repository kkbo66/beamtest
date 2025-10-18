#ifndef SHOWERENERGY_HH
#define SHOWERENERGY_HH

#include <map>
#include <vector>
#include "Parameter.hh"
#include "Shower.hh"

using namespace std;

class ShowerEnergy
{
  public:
  //Constructors and Destructors
  ShowerEnergy();
  ~ShowerEnergy();

  void Energy(Shower& aShower);
  private:
    Parameter& Para=Parameter::GetInstance();
};

#endif
