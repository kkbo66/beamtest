using namespace std;
using namespace TMath;
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<TLorentzVector> Vp4;

void drawenergy(string rootfile, double energy)
{

  gStyle->SetOptStat(0);
  vector<string> rootlist;
  rootlist.clear();
  if (rootfile.find(".txt") != string::npos)
  {
    ifstream infile;
    infile.open(rootfile.c_str());
    string line;
    while (getline(infile, line))
    {
      rootlist.push_back(line);
    }
  }
  else if (rootfile.find(".root") != string::npos)
  {
    rootlist.push_back(rootfile);
  }
  else
  {
    cout << "Please input root file" << endl;
    return;
  }

  string energy_str = to_string(int(energy)) + "MeV";

  TChain *t = new TChain("rec_data");
  for (unsigned int i = 0; i < rootlist.size(); i++)
  {
    t->Add(rootlist[i].c_str());
    cout << "Add root file: " << rootlist[i] << endl;
  }

  vector<int> *SeedID = 0;
  vector<int> *HitID = 0;
  vector<double> *Energy_5x5 = 0;
  vector<double> *Energy_Hit = 0;
  vector<double> *ShowerX = 0;
  vector<double> *ShowerY = 0;
  t->SetBranchAddress("ShowerID", &SeedID);
  t->SetBranchAddress("ShowerE5x5", &Energy_5x5);
  t->SetBranchAddress("HitID", &HitID);
  t->SetBranchAddress("HitEnergy", &Energy_Hit);
  t->SetBranchAddress("ShowerPosX5x5", &ShowerX);
  t->SetBranchAddress("ShowerPosY5x5", &ShowerY);

  //TH2F *hpos = new TH2F("hpos", "Shower Position", 100, -5, 5, 100, -5, 5);
  TH2F *hpos = new TH2F("hpos", "Shower Position", 100, -12.5, 12.5, 100, -12.5, 12.5);
  TH1F *hposx = new TH1F("hposx", "Shower X Position", 100, -12.5, 12.5);
  TH1F *hposy = new TH1F("hposy", "Shower Y Position", 100, -12.5, 12.5);

  double low = 0.0 * energy / 1000;
  // double high = 1.05*energy/1000;
  double high = 1.2 * energy / 1000;
  double seedcut = 0.2 * energy / 1000;
  TH1F *henergy_ecal = new TH1F("henergy_ecal", "ECAL Energy Distribution", 100, low, high);
  double perbin = (high - low) / 100.0;
  cout << t->GetEntries() << " entries in total." << endl;
  for (int i = 0; i < t->GetEntries(); i++)
  {
    //if(i<4000) continue;
    t->GetEntry(i);
    for (unsigned int j = 0; j < SeedID->size(); j++)
    {
      //if (SeedID->at(j) == 326034)
      if (true)
      { // center crystal ID for ECAL
        double seed_energy = -1;
        int hitnum = 0;
        for (unsigned int k = 0; k < HitID->size(); k++)
        {
          if (HitID->at(k) == 326034)
          {
            seed_energy = Energy_Hit->at(k) / 1000;
            // break;
          }
          else if (Energy_Hit->at(k) > 3)
          {
            hitnum++;
          }
        }
        // cout<<"Event: "<<i<<", Seed Energy: "<<seed_energy*1000<<" MeV"<< ", E5x5: "<<Energy_5x5->at(j)*1000<<" MeV"<<endl;
        //if (seed_energy < seedcut)
        //  continue;
        if (hitnum < 1)
          continue;
        // if(!(ShowerX->at(j)>-2 && ShowerX->at(j)<2 && ShowerY->at(j)<2 && ShowerY->at(j)>-2)) continue;
        henergy_ecal->Fill(Energy_5x5->at(j) / 1000);
        hpos->Fill(ShowerX->at(j), ShowerY->at(j));
        hposx->Fill(ShowerX->at(j));
        hposy->Fill(ShowerY->at(j));
      }
    }
  }

  TString pos_name = "c_pos_" + energy_str;
  TCanvas *c_pos = new TCanvas(pos_name, pos_name, 800, 600);
  c_pos->cd();
  gPad->SetGrid();
  hpos->SetXTitle("Shower X Position (cm)");
  hpos->SetYTitle("Shower Y Position (cm)");
  hpos->GetXaxis()->SetLabelSize(0.05);
  hpos->GetXaxis()->SetTitleSize(0.05);
  hpos->GetYaxis()->SetLabelSize(0.05);
  hpos->GetYaxis()->SetTitleSize(0.05);
  hpos->GetXaxis()->SetTitleOffset(0.9);
  hpos->GetYaxis()->SetTitleOffset(0.9);
  hpos->SetLineColor(kBlue);
  hpos->SetLineWidth(2);
  hpos->Draw("colz");
  TString posx_name = "c_posx_" + energy_str;
  TCanvas *c_posx = new TCanvas(posx_name, posx_name, 800, 600);
  c_posx->cd();
  gPad->SetGrid();
  hposx->SetXTitle("Shower X Position (cm)");
  hposx->SetYTitle("Entries");
  hposx->GetXaxis()->SetLabelSize(0.05);
  hposx->GetXaxis()->SetTitleSize(0.05);
  hposx->GetYaxis()->SetLabelSize(0.05);
  hposx->GetYaxis()->SetTitleSize(0.05);
  hposx->GetXaxis()->SetTitleOffset(0.9);
  hposx->GetYaxis()->SetTitleOffset(0.9);
  hposx->SetLineColor(kBlue);
  hposx->SetLineWidth(2);
  hposx->Draw("hist");
  TString posy_name = "c_posy_" + energy_str;
  TCanvas *c_posy = new TCanvas(posy_name, posy_name, 800, 600);
  c_posy->cd();
  gPad->SetGrid();
  hposy->SetXTitle("Shower Y Position (cm)");
  hposy->SetYTitle("Entries");
  hposy->GetXaxis()->SetLabelSize(0.05);
  hposy->GetXaxis()->SetTitleSize(0.05);
  hposy->GetYaxis()->SetLabelSize(0.05);
  hposy->GetYaxis()->SetTitleSize(0.05);
  hposy->GetXaxis()->SetTitleOffset(0.9);
  hposy->GetYaxis()->SetTitleOffset(0.9);
  hposy->SetLineColor(kBlue);
  hposy->SetLineWidth(2);
  hposy->Draw("hist");

  TString canvas_name = "c_" + energy_str;
  TCanvas *c = new TCanvas(canvas_name, canvas_name, 800, 600);
  c->cd();
  gPad->SetGrid();
  henergy_ecal->SetXTitle("Reconstructed ECAL Energy (GeV)");
  henergy_ecal->SetYTitle("Entries");
  henergy_ecal->GetXaxis()->SetLabelSize(0.05);
  henergy_ecal->GetXaxis()->SetTitleSize(0.05);
  henergy_ecal->GetYaxis()->SetLabelSize(0.05);
  henergy_ecal->GetYaxis()->SetTitleSize(0.05);
  henergy_ecal->GetXaxis()->SetTitleOffset(0.9);
  henergy_ecal->GetYaxis()->SetTitleOffset(0.9);
  henergy_ecal->SetLineColor(kBlue);
  henergy_ecal->SetLineWidth(2);
  henergy_ecal->Draw("hist");

}
