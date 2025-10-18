#ifndef SHOWERPOSITION_HH
#define SHOWERPOSITION_HH

#include <map>
#include <vector>
#include <TVector3.h>

#include "Shower.hh"

using namespace std;

class ShowerPosition
{
  public:
  //Constructors and Destructors
  ShowerPosition();
  ~ShowerPosition();
  
  void Position(Shower& aShower);
};
#endif
