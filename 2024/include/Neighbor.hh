#ifndef NEIGHBOR_HH
#define NEIGHBOR_HH

#include <vector>

using namespace std;

class Neighbor
{
  public:
   //Constructors and Destructors
   Neighbor();
   ~Neighbor();

   //3*3
   bool IsNeighbor(const int& ID1,const int& ID2);

   //5*5
   bool IsNextNeighbor(const int& ID1,const int& ID2);

   //3*3 8 ones
   vector<int> GetNeighbors(const int& ID);

   //5*5 16 ones
   vector<int> GetNextNeighbors(const int& ID);

   //5*5 25 ones
   vector<int> Get5x5Array(const int& ID);
};

#endif
