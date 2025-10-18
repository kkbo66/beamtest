#ifndef SHOWERSHAPE_HH
#define SHOWERSHAPE_HH

#include <map>
#include <vector>
#include <TVector3.h>

#include "Shower.hh"

using namespace std;

class ShowerShape
{
  public:
    ShowerShape();
    ~ShowerShape();

    void CalculateMoment(Shower& aShower) const;
    void SecondMoment(Shower& aShower) const;
    void LatMoment(Shower& aShower) const;
    void A20Moment(Shower& aShower) const;
    void A42Moment(Shower& aShower) const;

};
#endif

