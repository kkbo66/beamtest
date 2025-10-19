using namespace std;
using namespace TMath;

void changepos(TString file = "/home/kkbo/beamtest/2025/ECAL/parameter/HitPos.dat"){

  ifstream inFile(file);
  size_t pos = file.Last('/');
  TString outFileName = file(pos+1,file.Length()-pos);
  ofstream outFile(outFileName);

  string line;
  while (getline(inFile, line)) {
    istringstream iss(line);
    ostringstream oss;
    double number;
    int count = 0;
    while (iss >> number) {
      count++;
      if (count == 2) {
        number += 14.06; // Change the second number by adding 5.0
      }
      oss << number << " ";
    }
    outFile << oss.str() << endl;
  }
}
