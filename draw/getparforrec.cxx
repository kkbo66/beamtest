using namespace std;
using namespace TMath;

void getparforrec(){

  ifstream fmipl("par/forre/LGMipPeak.dat");
  ifstream fmiph("par/forre/HGMipPeak.dat");
  ifstream fnospedl("par/forre/lowgain_noise_stats.txt");
  ifstream fnospedh("par/forre/highgain_noise_stats.txt");
  ifstream fhlratio("par/forre/hlratio.txt");
  ifstream fhmax("par/forre/HighGainLimit.txt");

  vector<double> mipl, miph, nosl , nosh, pedl, pedh, hlratio, hmax;
  mipl.clear(); miph.clear(); nosl.clear(); nosh.clear(); pedl.clear(); pedh.clear(); hlratio.clear(); hmax.clear();

  double val;
  while(fmipl >> val) mipl.push_back(val);
  while(fmiph >> val) miph.push_back(val);
  while(fnospedl >> val) {
    pedl.push_back(val);
    fnospedl >> val;
    nosl.push_back(val);
  }
  while(fnospedh >> val) {
    pedh.push_back(val);
    fnospedh >> val;
    nosh.push_back(val);
  }
  string line;
  while(getline(fhlratio, line)){ 
    //Channel (1,1): Slope = 10.2259 ± 0.0112
    size_t pos1 = line.find("Slope = ");
    size_t pos2 = line.find(" ± ");
    string svalue = line.substr(pos1 + 8, pos2 - (pos1 + 8));
    val = atof(svalue.c_str());
    hlratio.push_back(val);
  }
  while(fhmax >> val) hmax.push_back(val);
  fmipl.close(); fmiph.close(); fnospedl.close(); fnospedh.close(); fhlratio.close(); fhmax.close();


  ofstream fout("CaliPara.dat");
  for(int i=0; i< pedl.size(); i++){
    fout << pedl[i] << "\t" << nosl[i] << "\t" 
         << pedh[i] << "\t" << nosh[i] << "\t" 
         << hlratio[i] << "\t" << hmax[i] << endl;
  }
  ofstream fout2("LGMipPeak.dat");
  for(int i=0; i< mipl.size(); i++){
    fout2 << mipl[i]+pedl[i] << endl;
  }
  ofstream fout3("HGMipPeak.dat");
  for(int i=0; i< miph.size(); i++){
    fout3 << miph[i]+pedh[i] << endl;
  }
  ofstream fout4("Pedestal.dat");
  for(int i=0; i< pedl.size(); i++){
    fout4 << pedl[i] << "\t" << nosl[i] << "\t" << nosl[i] << "\t"
          << pedh[i] << "\t" << nosh[i] << "\t" << nosh[i] << endl;
  }
  fout.close(); fout2.close(); fout3.close(); fout4.close();

}
