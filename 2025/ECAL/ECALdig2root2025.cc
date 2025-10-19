#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "TString.h"
#include "data_model.hh"
#include "Decode2025.hh"

using namespace std;

bool isTextFile(const string &fileName)
{
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos == std::string::npos)
        return false;

    std::string extension = fileName.substr(dotPos + 1);
    if (extension == "txt")
        return true;
    else
        return false;
}

bool isRootFile(const string &fileName)
{
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos == std::string::npos)
        return false;

    std::string extension = fileName.substr(dotPos + 1);
    if (extension == "root")
        return true;
    else
        return false;
}

// bool nameChanger(const string& fileName, string& name){
string nameChanger(const string &fileName)
{
    string rootfilename;

    size_t lastSlashPos = fileName.find_last_of("/");
    if (lastSlashPos != std::string::npos)
        rootfilename = fileName.substr(lastSlashPos + 1);
    else
        rootfilename = fileName;

    size_t dotPos = rootfilename.find_last_of('.');
    if (dotPos != std::string::npos)
        rootfilename = rootfilename.substr(0, dotPos) + ".root";
    else
        rootfilename = rootfilename + ".root";
    // cout<< "File name does not contain '.txt'" << std::endl;

    return rootfilename;
    // name=rootfilename;
    // return true;
}

int main(int argc, char const *argv[])
{

    vector<string> datafiles;

    if (isTextFile(argv[1]))
    {
        ifstream filestream(argv[1]);
        if (filestream.is_open())
        {
            string filename;
            while (getline(filestream, filename))
                datafiles.push_back(filename);
        }
        else
            cout << "error in reading filelist  " << endl;
    }
    else
        datafiles.push_back(argv[1]);

    vector<string> rootfiles;
    if (argc == 3)
    {
        if (isRootFile(argv[2]))
            rootfiles.push_back(string(argv[2]));
    }
    else
    {
        if (isTextFile(argv[1]))
        {
            for (size_t i = 0; i < datafiles.size(); i++)
            {
                rootfiles.push_back(nameChanger(datafiles.at(i)));
                cout << "Auto save file as: " << rootfiles.at(i) << endl;
            }
        }
        else
        {
            rootfiles.push_back("re.root");
            cout << "Auto save file as re.root..." << endl;
        }
    }

    if (rootfiles.size() == 1)
    {
        Decode2025 *De = new Decode2025(rootfiles.at(0));

        for (size_t i = 0; i < datafiles.size(); i++)
        {
            ifstream indata(datafiles.at(i), ios::binary);
            if (!indata.good())
            {
                cout << "error in reading file: " << datafiles.at(i) << endl;
                return -1;
            }
            // 直传格式
            // De->GetHit(indata);
            // DAQ格式
            De->GetHitDAQ(indata);
            indata.close();
        }
        delete De;
    }
    else
    {
        for (size_t i = 0; i < datafiles.size(); i++)
        {
            Decode2025 *De = new Decode2025(rootfiles.at(i));
            ifstream indata(datafiles.at(i), ios::binary);
            if (!indata.good())
            {
                cout << "error in reading file: " << datafiles.at(i) << endl;
                return -1;
            }
            // De->GetHit(indata);
            De->GetHitDAQ(indata);
            indata.close();
            delete De;
        }
    }

    return 0;
}
