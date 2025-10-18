#include "Neighbor.hh"

//Constructors and Destructors
Neighbor::Neighbor()
{

}

Neighbor::~Neighbor()
{

}

//3*3
bool Neighbor::IsNeighbor(const int& ID1,const int& ID2)
{
  bool is=false;
  vector<int> NeighborVec;
  vector<int>::const_iterator citer;

  NeighborVec=GetNeighbors(ID1);
  for(citer=NeighborVec.begin();citer!=NeighborVec.end();citer++){
    if((*citer==ID2)) {
      is=true;
      break;
    }
  }

  return is;
}

//5*5
bool Neighbor::IsNextNeighbor(const int& ID1,const int& ID2)
{
  bool is=false;
  vector<int> NeighborVec;
  vector<int>::const_iterator citer;

  NeighborVec=GetNextNeighbors(ID1);
  for(citer=NeighborVec.begin();citer!=NeighborVec.end();citer++){
    if((*citer==ID2)) {
      is=true;
      break;
    }
  }

  return is;
}

//3*3 8 ones
vector<int> Neighbor::GetNeighbors(const int& ID)
{
  unsigned int M=ID/100000;
  unsigned int R=(ID%100000)/1000;
  unsigned int C=(ID%100000)%1000;

  int NbID;
  vector<int> NeighborVec;
  NeighborVec.clear();

  if(M==3){
    unsigned int RL=0;
    unsigned int RR=0;
    unsigned int CU=0;
    unsigned int CD=0;

    if(R==24){
      RR=R+1;
      NbID=M*100000+RR*1000+C;
      NeighborVec.push_back(NbID);
    }  

    if(R==28){
      RL=R-1;
      NbID=M*100000+RL*1000+C;
      NeighborVec.push_back(NbID);
    }

    if(R==25||R==26||R==27){
      RR=R+1;
      RL=R-1;
      NbID=M*100000+RR*1000+C;
      NeighborVec.push_back(NbID);   
      NbID=M*100000+RL*1000+C;
      NeighborVec.push_back(NbID);
    }

    if(C==32){
      CD=C+1;
      NbID=M*100000+R*1000+CD;
      NeighborVec.push_back(NbID);
      if(RL==0){
        NbID=M*100000+RR*1000+CD;
        NeighborVec.push_back(NbID);     
      } 
      if (RR==0){  
        NbID=M*100000+RL*1000+CD;
        NeighborVec.push_back(NbID);
      }

      if((RL!=0)&&(RR!=0)){
        NbID=M*100000+RR*1000+CD;
        NeighborVec.push_back(NbID); 
        NbID=M*100000+RL*1000+CD;
        NeighborVec.push_back(NbID);
      }
    }

    if(C==36){
      CU=C-1;
      NbID=M*100000+R*1000+CU;
      NeighborVec.push_back(NbID);
      if(RL==0){
        NbID=M*100000+RR*1000+CU;
        NeighborVec.push_back(NbID);     
      } 
      if (RR==0){  
        NbID=M*100000+RL*1000+CU;
        NeighborVec.push_back(NbID);
      }

      if((RL!=0)&&(RR!=0)){
        NbID=M*100000+RR*1000+CU;
        NeighborVec.push_back(NbID); 
        NbID=M*100000+RL*1000+CU;
        NeighborVec.push_back(NbID);
      }
    }

    if(C==33||C==34||C==35){
      CU=C-1;
      CD=C+1;
      NbID=M*100000+R*1000+CU;
      NeighborVec.push_back(NbID);   
      NbID=M*100000+R*1000+CD;
      NeighborVec.push_back(NbID); 
      if(RL==0){
        NbID=M*100000+RR*1000+CU;
        NeighborVec.push_back(NbID);   
        NbID=M*100000+RR*1000+CD;
        NeighborVec.push_back(NbID);   
      }
      if(RR==0){
        NbID=M*100000+RL*1000+CU;
        NeighborVec.push_back(NbID);   
        NbID=M*100000+RL*1000+CD;
        NeighborVec.push_back(NbID);
      } 
      if((RL!=0)&&(RR!=0)){
        NbID=M*100000+RR*1000+CU;
        NeighborVec.push_back(NbID);   
        NbID=M*100000+RR*1000+CD;
        NeighborVec.push_back(NbID); 
        NbID=M*100000+RL*1000+CU;
        NeighborVec.push_back(NbID);   
        NbID=M*100000+RL*1000+CD;
        NeighborVec.push_back(NbID);
      }  
    }  
  }

  return NeighborVec;
}

//5*5 16 ones
vector<int> Neighbor::GetNextNeighbors(const int& ID)
{
  unsigned int M=ID/100000;

  vector<int> NeighborVec;
  NeighborVec.clear();

  if(M==3){
    vector<int> tmpNeighborVec;
    tmpNeighborVec.clear();
    vector<int> tmpNextNeighborVec;
    tmpNextNeighborVec.clear();
    vector<int>::const_iterator citer; 
    vector<int>::const_iterator ctmpiter; 
    vector<int>::const_iterator ctmpiter1; 
    vector<int>::const_iterator ctmpnextiter; 
    bool switch1=false;
    bool switch2=false;


    tmpNeighborVec=GetNeighbors(ID);
    for(ctmpiter=tmpNeighborVec.begin();ctmpiter!=tmpNeighborVec.end();ctmpiter++){
      tmpNextNeighborVec.clear();
      tmpNextNeighborVec=GetNeighbors(*ctmpiter);

      for(ctmpnextiter=tmpNextNeighborVec.begin();ctmpnextiter!=tmpNextNeighborVec.end();ctmpnextiter++){
        for(citer=NeighborVec.begin();citer!=NeighborVec.end();citer++){
          if(*ctmpnextiter==*citer){ //this crystal is already included
            switch1=true;
            break;
          }
        }

        if(!switch1){ //find a new crystal
          for(ctmpiter1=tmpNeighborVec.begin();ctmpiter1!=tmpNeighborVec.end();ctmpiter1++){
            if(*ctmpnextiter==*ctmpiter1){ //this crystal belongs to Neighbor
              switch2=true;
              break;
            }
          }

          if(*ctmpnextiter==ID)
            switch2=true;
          if(!switch2)
            NeighborVec.push_back(*ctmpnextiter);
          else
            switch2=false;

        }
        else
          switch1=false;
      }
    }
  }

  return NeighborVec;
}

//5*5 25 ones
vector<int> Neighbor::Get5x5Array(const int& ID)
{
  if(ID!=0){
    vector<int> ID3x3=GetNeighbors(ID);
    vector<int> ID5x5=GetNextNeighbors(ID);
    vector<int>::iterator iter;

    ID5x5.push_back(ID);
    for(iter=ID3x3.begin();iter!=ID3x3.end();iter++) ID5x5.push_back(*iter);

    return ID5x5;

  }
  else{
    vector<int> ID5x5;
    ID5x5.clear();
    return ID5x5;
  }
}
