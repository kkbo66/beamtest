#include "ShowerEnergy.hh"
#include "RecHit.hh"
#include "Neighbor.hh"

//Constructors and Destructors
ShowerEnergy::ShowerEnergy()
{

}

ShowerEnergy::~ShowerEnergy()
{

}

void ShowerEnergy::Energy(Shower& aShower)
{
  double E1=0;
  double E9=0;
  double E25=0;
  double EAll=0;
  map<int,RecHit>::const_iterator citer;
  
  Neighbor Ngh;

  int SeedID=aShower.SeedID();

  vector<int> ID3x3=Ngh.GetNeighbors(SeedID);
  vector<int> ID5x5=Ngh.GetNextNeighbors(SeedID);
  vector<int> tmpID3x3;
  vector<int> tmpID5x5;
  vector<int>::iterator iter1;
  vector<int>::iterator iter2;

  tmpID3x3.push_back(SeedID);
  tmpID5x5.push_back(SeedID);

  citer=aShower.Find(SeedID);
  E1=(citer->second.Energy())*(citer->second.Weight());
  E9+=(citer->second.Energy())*(citer->second.Weight());
  E25+=(citer->second.Energy())*(citer->second.Weight());



  for(iter1=ID3x3.begin();iter1!=ID3x3.end();iter1++){
    citer=aShower.Find(*iter1);
    if(citer!=aShower.End()){
      tmpID3x3.push_back(*iter1);
      tmpID5x5.push_back(*iter1);
      E9+=(citer->second.Energy())*(citer->second.Weight());
      E25+=(citer->second.Energy())*(citer->second.Weight());
    }
  }

  for(iter2=ID5x5.begin();iter2!=ID5x5.end();iter2++){
    citer=aShower.Find(*iter2);
    if(citer!=aShower.End()){
      tmpID5x5.push_back(*iter2);
      E25+=(citer->second.Energy())*(citer->second.Weight());
    }
  }

  for(citer=aShower.Begin();citer!=aShower.End();citer++){
    EAll+=(citer->second.Energy())*(citer->second.Weight());
  }

  /*//energy correction
  int M = SeedID / 100000;
  int R = (SeedID % 100000) / 1000;
  if(M==3){
    if(R<=26) R=R+9;
    else R=35+26-R;
  }
  if(M==1||M==2) R=R-1;
  double ECor = Para.ECorMC(E25,R);

  //energy error
  double err=Para.ErrMC(E25,R);
  double de=err/100.0*ECor;*/
  
  
  aShower.setNumHits(aShower.ShowerSize());
  aShower.setID3x3(tmpID3x3);
  aShower.setID5x5(tmpID5x5);
  aShower.setESeed(E1);
  aShower.setE3x3(E9);
  aShower.setE5x5(E25);
  aShower.setEAll(EAll);
  //aShower.setEnergy(ECor);
  //aShower.setDE(de);
}
