#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int Hex2dec(unsigned short tmp){
 unsigned short tmpbit=0;
 tmpbit = tmp>>8;
 tmp =tmp-tmpbit*256;
 tmp=tmp*256+tmpbit;
 return tmp;
}

int main(int argc, char const *argv[])
{
  ifstream indata(argv[1],ios::binary);
  if(!indata.good()){
  	cout<<"error in reading file"<<endl; 
	return-1;
  }

  indata.seekg(0,indata.end);
  long long int length =indata.tellg();
  indata.seekg(0,indata.beg);

  unsigned short *tmp = new unsigned short();

  int ns=0;

  while(indata.tellg()<(length-16*2)&&indata.tellg()>-1){
  indata.read((char*)tmp,sizeof(unsigned short));
  if(*tmp==0x8603) {
  indata.read((char*)tmp,sizeof(unsigned short));
  cout<<*tmp<<endl;
  indata.seekg(8*2,indata.cur);
  
  unsigned long *timecode = new unsigned long();
  indata.read((char*)timecode,sizeof(unsigned long));

  //cout<<"timecode: "<<(*timecode>>2)<<endl;
  cout<<"timecode: "<<(*timecode)<<endl;
  
  

  }
  }




  return 0;

}
