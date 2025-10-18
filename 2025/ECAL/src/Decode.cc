#include "Decode.hh"

#include <iostream>
#include <fstream>

using namespace std;
using namespace TMath;

#define npoints  256
#define nnoise	 25
// set timebits to adjust for data type
#define timebits 12
#define timewinb 40 
#define timewine 80

//#define timewinb 16
//#define timewine 48

//#define timewinb 170
//#define timewine 210


/*int hex2dec(unsigned short tmp){

  tmp = tmp>>2;
  return tmp;
  }*/

int hex2dec(unsigned short tmp){
  unsigned short tmpbit = 0;
  tmpbit = tmp>>8;
  tmp = tmp - tmpbit*256;
  tmp = (tmp*256+tmpbit)>>2;
  return tmp;
}
int hex2dect(unsigned short tmp){
  unsigned short tmpbit = 0;
  tmpbit = tmp>>8;
  tmp = tmp - tmpbit*256;
  tmp = (tmp*256+tmpbit);
  //tmp=((tmp>>8)&0x00FF)|((tmp<<8)&0xFF00);
  return tmp;
}
int hex2decs(unsigned short tmp){
  //unsigned short Tmp=hex2dect(tmp);
  unsigned short Tmp=tmp;
  unsigned short tmpbit = 0;
  tmpbit = Tmp>>14;
  Tmp = Tmp - tmpbit*16384;
  Tmp = Tmp*4;
  return Tmp;
}

bool IsComplete(ifstream& indata){
  unsigned short *tmp1 = new unsigned short();
  unsigned short *tmp2 = new unsigned short();
  unsigned short *tmp3 = new unsigned short();
  unsigned short *tmp4 = new unsigned short();
  unsigned short *tmp5 = new unsigned short();
  unsigned short *tmp6 = new unsigned short();
  unsigned short *tmp7 = new unsigned short();
  unsigned short *tmp8 = new unsigned short();

  indata.read((char*)tmp1,sizeof(unsigned short));
  indata.read((char*)tmp2,sizeof(unsigned short));
  indata.read((char*)tmp3,sizeof(unsigned short));
  indata.read((char*)tmp4,sizeof(unsigned short));

  //cout<<"HID: "<<*tmp2<<endl;

  if(hex2dect(*tmp1) == 0x0606 && hex2dect(*tmp3) == 0x01eb && hex2dect(*tmp4) == 0xaa55){
    indata.seekg((npoints*12+timebits)*2, indata.cur);
    indata.read((char*)tmp5,sizeof(unsigned short));
    indata.read((char*)tmp6,sizeof(unsigned short));
    indata.read((char*)tmp7,sizeof(unsigned short));
    indata.read((char*)tmp8,sizeof(unsigned short));

    //if(hex2dect(*tmp8) == 0x55aa && hex2dect(*tmp7) == 0x01eb && hex2dect(*tmp6) == 0x55aa && hex2dect(*tmp5) == 0x03eb){
    if(hex2dect(*tmp8) == 0x55aa && hex2dect(*tmp6) == 0x55aa && hex2dect(*tmp5) == 0x03eb){
      return true;
    }
    else{
      indata.seekg(-(npoints*12+timebits+4)*2, indata.cur);
      return false;
    }

  }
  return false;
}

bool clear(float (&mampl)[6][npoints],float (&mamph)[6][npoints],float (&mnoil)[6][nnoise],float (&mnoih)[6][nnoise],float (&mlplat)[6],float (&mhplat)[6],float (&mlpeak)[6],float (&mhpeak)[6]){
  for(int i=0;i<6;i++){
    for(int j=0;j<256;j++){
      mampl[i][j]=0;
      mamph[i][j]=0;
      if(j<nnoise)  mnoil[i][j]=0;
      if(j<nnoise)  mnoih[i][j]=0;
    }
  }
  for(int i=0;i<6;i++){
    mlplat[i]=0;mhplat[i]=0;mlpeak[i]=0;mhpeak[i]=0;
  }

  return true;
}

bool ReadState(ifstream& indata,float (&tmpera)[10], float &voltage,float &current){
  float da=0;
  unsigned short *tmp = new unsigned short();
  for(int i=0;i<12;i++){
    indata.read((char*)tmp, sizeof(unsigned short));
    da=hex2decs(*tmp);
    //cout<<std::dec<<i<<" "<<std::hex<<*tmp<<"  "<<std::dec<<da<<"   "<<da/65536*1.15<<endl;;
    if(i<4) tmpera[3-i]=((da/65536*1.15)*1000-943.227)/-5.194+30;
    else if(i<8) tmpera[11-i]=((da/65536*1.15)*1000-943.227)/-5.194+30;
    else if(i>9) tmpera[19-i]=((da/65536*1.15)*1000-943.227)/-5.194+30;
    //else if(i>9) tmpera[19-i]=(da/65536*1.15)/2*800;

    if(i==8) current=(da/65536*1.15)/2*50;
    if(i==9) voltage=(da/65536*1.15)/2*800;
    //if(i==9) cout<<std::hex<<*tmp<<"  "<<voltage<<endl;
  }

  return true;
}

bool ReadData(ifstream& indata, float (&mampl)[6][npoints],float (&mamph)[6][npoints],float (&mnoil)[6][nnoise],float (&mnoih)[6][nnoise],float (&mlplat)[6],float (&mhplat)[6],float (&mlpeak)[6],float (&mhpeak)[6]){
  int data;
  unsigned short *tmp = new unsigned short();
  int channel;
  for(int i=0;i<npoints*12;i++){
    if(i<npoints*4) channel= 1;
    else if(i<npoints*8) channel =3;
    else channel =5;

    indata.read((char*)tmp, sizeof(unsigned short));
    //data=hex2dec(*tmp);
    data=(*tmp)/4;

    if(i%4==3){
      mamph[channel-1][i/4-256*(channel/2)]=(data);
      if((i/4)<(nnoise+256*(channel/2))){
        mhplat[channel-1] += mamph[channel-1][i/4-256*(channel/2)];

        mnoih[channel-1][i/4-256*(channel/2)] = mamph[channel-1][i/4-256*(channel/2)];
      }
      if(mamph[channel-1][i/4-256*(channel/2)]>mhpeak[channel-1]&&(i/4-256*(channel/2))>timewinb&&(i/4-256*(channel/2))<timewine) mhpeak[channel-1] = mamph[channel-1][i/4-256*(channel/2)];
    }

    if(i%4==2){
      mampl[channel-1][i/4-256*(channel/2)]=(data);
      if((i/4)<(nnoise+256*(channel/2))){
        mlplat[channel-1] += mampl[channel-1][i/4-256*(channel/2)];

        mnoil[channel-1][i/4-256*(channel/2)] = mampl[channel-1][i/4-256*(channel/2)];
      }
      if(mampl[channel-1][i/4-256*(channel/2)]>mlpeak[channel-1]&&(i/4-256*(channel/2))>timewinb&&(i/4-256*(channel/2))<timewine) mlpeak[channel-1] = mampl[channel-1][i/4-256*(channel/2)];
    }

    if(i%4==1){
      //cout<<"hex: "<<hex<<data*4<<endl;
      mamph[channel][i/4-256*(channel/2)]=(data);
      if((i/4)<(nnoise+256*(channel/2))){
        mhplat[channel] += mamph[channel][i/4-256*(channel/2)];

        mnoih[channel][i/4-256*(channel/2)] = mamph[channel][i/4-256*(channel/2)];
      }
      if(mamph[channel][i/4-256*(channel/2)]>mhpeak[channel]&&(i/4-256*(channel/2))>timewinb&&(i/4-256*(channel/2))<timewine) mhpeak[channel] = mamph[channel][i/4-256*(channel/2)];
    }

    if(i%4==0){
      mampl[channel][i/4-256*(channel/2)]=(data);
      if((i/4)<(nnoise+256*(channel/2))){
        mlplat[channel] += mampl[channel][i/4-256*(channel/2)];

        mnoil[channel][i/4-256*(channel/2)] = mampl[channel][i/4-256*(channel/2)];
      }
      if(mampl[channel][i/4-256*(channel/2)]>mlpeak[channel]&&(i/4-256*(channel/2))>timewinb&&(i/4-256*(channel/2))<timewine) mlpeak[channel] = mampl[channel][i/4-256*(channel/2)];
    }    
  }

  return true; 
}


Decode::Decode(string filename):Hit(25)
{

  for(int i=0;i<25;i++) Hit[i]= new decode_data_col();

  f = new TFile(filename.data(),"recreate");
  T = new TTree("decode_data","decode_data");

  T->Branch("EventID",&EventID,"EventID/L");
  T->Branch("TriggerID",&TriggerID,"TriggerID/I");
  T->Branch("TimeCode",&TimeCode,"TimeCode/L");
  T->Branch("Time",&Time,Form("Time[%d]/F",256));
  for(int i=0;i<5;i++){
    string name="Voltage_"+to_string(i+1);
    T->Branch(name.data(),&Voltage[i],Form("Voltage%d/F",i));
  }

  for(int i=0;i<5;i++){
    string name="Current_"+to_string(i+1);
    T->Branch(name.data(),&Current[i],Form("Current%d/F",i));
  }
  
  string Name[25];
  for(int i=0;i<25;i++){
    int no;
    if(i%5==0) no=3; 
    if(i%5==1) no=4; 
    if(i%5==2) no=5; 
    if(i%5==3) no=1; 
    if(i%5==4) no=2; 
    Name[i]=to_string(i/5+1)+"_"+to_string(no);
  }
  for(int i=0;i<25;i++){
    //string name="Hit_"+to_string(i+1);
    string name="Hit_"+Name[i];
    string leaf_list = "";

    leaf_list += "CrystalID/L";
    leaf_list += ":Temperature1/D";
    leaf_list += ":Temperature2/D";
    leaf_list += ":LAmplitude[256]/D";
    leaf_list += ":HAmplitude[256]/D";
    leaf_list += ":LNoise[25]/D";
    leaf_list += ":HNoise[25]/D";
    leaf_list += ":LowGainPedestal/D";
    leaf_list += ":HighGainPedestal/D";
    leaf_list += ":LowGainPeak/D";
    leaf_list += ":HighGainPeak/D";

    T->Branch(name.data(),&Hit[i]->CrystalID, leaf_list.data());
  }
}

Decode::~Decode(){
  if(f!=nullptr){
    f->cd();
    T->Write();
  }
  f->Close();
  delete f;
  f=nullptr;
}

void Decode::GetHit(ifstream& indata){

  float mampl[6][npoints];
  float mamph[6][npoints];
  float mnoil[6][nnoise];
  float mnoih[6][nnoise];

  float mlplat[6],mhplat[6],mlpeak[6],mhpeak[6];

  for(int i=0;i<6;i++){
    mlplat[i]=0;mhplat[i]=0;mlpeak[i]=0;mhpeak[i]=0;
  }

  indata.seekg(0, indata.end);
  //long long int length = indata.tellg()/500;
  long long int length = indata.tellg();
  indata.seekg(0, indata.beg);

  float lastoutputprogress=0.0f;

  unsigned short *tmp = new unsigned short();

  Long64_t nsignal=0;
  Long64_t ns=0;

  while(indata.tellg()<(length-(npoints*12+timebits+8)*5*2)&&indata.tellg()>-1){

    float progress = static_cast<float>(indata.tellg())/length*100;
    if(abs(progress-5*static_cast<int>(progress/5))<0.1&&progress!=lastoutputprogress){
     cout<<"Progress: "<< progress<<"%\r"<<endl;
     std::cout.flush();   
     lastoutputprogress=progress;  
    }


    indata.read((char*)tmp,sizeof(unsigned short));
    //cout<<*tmp<<"  "<<hex2dect(*tmp)<<endl;
    //cout<<hex2dect(*tmp)<<"  "<<hex2dec(*tmp)<<endl;
    if(hex2dect(*tmp) == 0xd778){
      //if(hex2dect(*tmp) == 0x0f00){

      indata.read((char*)tmp,sizeof(unsigned short));
      if(hex2dect(*tmp) == 0x2211){

	  //cout<<ns<<" "<<indata.tellg()<<endl;
        
        ns++;

        indata.seekg(-3*2,indata.cur);
        indata.read((char*)tmp,sizeof(unsigned short));
        //cout<<*tmp<<"  "<<std::hex<<hex2dect(*tmp)<<endl;
        //cout<<"LID: "<<*tmp<<endl;
        //cout<<*tmp<<" ";
        TriggerID=*tmp; 
        indata.read((char*)tmp,sizeof(unsigned short));
        indata.read((char*)tmp,sizeof(unsigned short));

        bool flag1=IsComplete(indata);
        if(flag1==false) continue;
        bool flag2=IsComplete(indata);
        if(flag2==false) continue;
        bool flag3=IsComplete(indata);
        if(flag3==false) continue;
        bool flag4=IsComplete(indata);
        if(flag4==false) continue;
        bool flag5=IsComplete(indata);
        if(flag5==false) continue;

        nsignal++;
        //cout<<"ID"<<"  "<<std::dec<<nsignal<<endl;
        EventID=nsignal;
        for(int i=0;i<256;i++) Time[i]=i*12.5;

        int CrystalID=0;
        int M=0;
        int R=0;
        int C=0;

        //indata.seekg(-(npoints*12+timebits+8)*4*2-(npoints*12+4)*2,indata.cur);

        //indata.seekg(-(npoints*12+timebits+4)*1*2,indata.cur);

        //Goodone
        indata.seekg(-(npoints*12+timebits+8)*4*2-(npoints*12+timebits+4)*2,indata.cur);
        memset(Temperature,0,sizeof(Temperature));

        ReadState(indata,Temperature,Voltage[0],Current[0]);

        float mampl[6][npoints];
        float mamph[6][npoints];
        float mnoil[6][nnoise];
        float mnoih[6][nnoise];
        float mlplat[6],mhplat[6],mlpeak[6],mhpeak[6];

        clear(mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        ReadData(indata,mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        for(int i=0;i<6;i++){
          mlplat[i]=mlplat[i]/nnoise;mhplat[i]=mhplat[i]/nnoise;
        }

        M=24;

        for(int i=0;i<6;i++){
          float rowal[256],rowah[256],rownl[nnoise],rownh[nnoise],tmpera[2];
          if(i==3) continue;
          for(int j=0;j<256;j++){
            rowal[j]=mampl[i][j];
            rowah[j]=mamph[i][j];
            if(j<nnoise)rownl[j]=mnoil[i][j];
            if(j<nnoise)rownh[j]=mnoih[i][j];
          }
          if(i<3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*i];}
          if(i>3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*(i-1)];}
          if(i==0) C=34;
          if(i==1) C=33;
          if(i==2) C=32;
          if(i==4) C=36;
          if(i==5) C=35;
          CrystalID=3*100000+M*1000+C;
          if(i<3) Hit[i]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
          if(i>3) Hit[i-1]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
        }
        
        M=25;

        //第2块板
        indata.seekg(8*2, indata.cur);
        memset(Temperature,0,sizeof(Temperature));

        ReadState(indata,Temperature,Voltage[1],Current[1]);

        clear(mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        ReadData(indata,mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        for(int i=0;i<6;i++){
          mlplat[i]=mlplat[i]/nnoise;mhplat[i]=mhplat[i]/nnoise;
        }

        for(int i=0;i<6;i++){
          float rowal[256],rowah[256],rownl[nnoise],rownh[nnoise],tmpera[2];
          if(i==3) continue;
          for(int j=0;j<256;j++){
            rowal[j]=mampl[i][j];
            rowah[j]=mamph[i][j];
            if(j<nnoise)rownl[j]=mnoil[i][j];
            if(j<nnoise)rownh[j]=mnoih[i][j];
          }
          if(i<3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*i];}
          if(i>3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*(i-1)];}
          if(i==0) C=34;
          if(i==1) C=33;
          if(i==2) C=32;
          if(i==4) C=36;
          if(i==5) C=35;
          CrystalID=3*100000+M*1000+C;
          if(i<3) Hit[i+5]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
          if(i>3) Hit[i+5-1]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
        }
        
        M=26;

        //第3块板
        indata.seekg(8*2, indata.cur);
        memset(Temperature,0,sizeof(Temperature));

        ReadState(indata,Temperature,Voltage[2],Current[2]);

        clear(mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        ReadData(indata,mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        for(int i=0;i<6;i++){
          mlplat[i]=mlplat[i]/nnoise;mhplat[i]=mhplat[i]/nnoise;
        }

        for(int i=0;i<6;i++){
          float rowal[256],rowah[256],rownl[nnoise],rownh[nnoise],tmpera[2];
          if(i==3) continue;
          for(int j=0;j<256;j++){
            rowal[j]=mampl[i][j];
            rowah[j]=mamph[i][j];
            if(j<nnoise)rownl[j]=mnoil[i][j];
            if(j<nnoise)rownh[j]=mnoih[i][j];
          }
          if(i<3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*i];}
          if(i>3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*(i-1)];}
          if(i==0) C=34;
          if(i==1) C=33;
          if(i==2) C=32;
          if(i==4) C=36;
          if(i==5) C=35;
          CrystalID=3*100000+M*1000+C;
          if(i<3) Hit[i+10]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
          if(i>3) Hit[i+10-1]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
        }
        
        M=27;

        //第4块板
        indata.seekg(8*2, indata.cur);
        memset(Temperature,0,sizeof(Temperature));

        ReadState(indata,Temperature,Voltage[3],Current[3]);

        clear(mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        ReadData(indata,mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        for(int i=0;i<6;i++){
          mlplat[i]=mlplat[i]/nnoise;mhplat[i]=mhplat[i]/nnoise;
        }

        for(int i=0;i<6;i++){
          float rowal[256],rowah[256],rownl[nnoise],rownh[nnoise],tmpera[2];
          if(i==3) continue;
          for(int j=0;j<256;j++){
            rowal[j]=mampl[i][j];
            rowah[j]=mamph[i][j];
            if(j<nnoise)rownl[j]=mnoil[i][j];
            if(j<nnoise)rownh[j]=mnoih[i][j];
          }
          if(i<3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*i];}
          if(i>3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*(i-1)];}
          if(i==0) C=34;
          if(i==1) C=33;
          if(i==2) C=32;
          if(i==4) C=36;
          if(i==5) C=35;
          CrystalID=3*100000+M*1000+C;
          if(i<3) Hit[i+15]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
          if(i>3) Hit[i+15-1]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
        }        
        
        M=28;

        //第5块板
        indata.seekg(8*2, indata.cur);
        memset(Temperature,0,sizeof(Temperature));

        ReadState(indata,Temperature,Voltage[4],Current[4]);

        clear(mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        ReadData(indata,mampl,mamph,mnoil,mnoih,mlplat,mhplat,mlpeak,mhpeak);

        for(int i=0;i<6;i++){
          mlplat[i]=mlplat[i]/nnoise;mhplat[i]=mhplat[i]/nnoise;
        }

        for(int i=0;i<6;i++){
          float rowal[256],rowah[256],rownl[nnoise],rownh[nnoise],tmpera[2];
          if(i==3) continue;
          for(int j=0;j<256;j++){
            rowal[j]=mampl[i][j];
            rowah[j]=mamph[i][j];
            if(j<nnoise)rownl[j]=mnoil[i][j];
            if(j<nnoise)rownh[j]=mnoih[i][j];
          }
          if(i<3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*i];}
          if(i>3) {for(int j=0;j<2;j++) tmpera[j]=Temperature[j+2*(i-1)];}
          if(i==0) C=34;
          if(i==1) C=33;
          if(i==2) C=32;
          if(i==4) C=36;
          if(i==5) C=35;
          CrystalID=3*100000+M*1000+C;
          if(i<3) Hit[i+20]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);
          if(i>3) Hit[i+20-1]->Set(CrystalID,tmpera[0],tmpera[1],rowal,rowah,rownl,rownh,mlplat[i],mhplat[i],mlpeak[i],mhpeak[i]);       
        }


        indata.seekg(4*2, indata.cur);
        unsigned long *timecode = new unsigned long();
        indata.read((char*)timecode,sizeof(unsigned long));
        //cout<<"TimeCode"<<"   "<<*timecode<<endl;
        //cout<<(*timecode>>8)<<endl;
        TimeCode=(*timecode>>8);
        T->Fill();

      }
    }
    }

    cout<<"Length of the file is : "<< length<< "\tNumber of heads is: "<< ns << "\tNumber of signals is: " << nsignal << endl;
  }
