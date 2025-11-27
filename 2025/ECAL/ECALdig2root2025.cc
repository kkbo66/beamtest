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
    if (argc >= 3)
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
            // rootfiles.push_back("decode.root");
            rootfiles.push_back(nameChanger(datafiles.at(0)));
            cout << "Auto save file as: " << rootfiles.at(0) << endl;
        }
    }
    // true：更换1、2、3列电子学通道顺序
    bool ChannelInvert;
    if (argc >= 4)
    {
        if (std::stoi(argv[3]) == 1)
            ChannelInvert = true;
        else
            ChannelInvert = false;
    }
    if (rootfiles.size() == 1)
    {
        Decode2025 *De = new Decode2025(rootfiles.at(0));
        De->InvertChannel(ChannelInvert);

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
            // De->GetHitDAQ(indata);
            // 2023run46之后解码包含在线TQ信息
            // 在线格式
            De->GetHitOnline(indata);
            indata.close();
        }
        delete De;
    }
    else
    {
        for (size_t i = 0; i < datafiles.size(); i++)
        {
            Decode2025 *De = new Decode2025(rootfiles.at(i));
            De->InvertChannel(ChannelInvert);
            ifstream indata(datafiles.at(i), ios::binary);
            if (!indata.good())
            {
                cout << "error in reading file: " << datafiles.at(i) << endl;
                return -1;
            }
            // De->GetHit(indata);
            // De->GetHitDAQ(indata);
            De->GetHitOnline(indata);
            indata.close();
            delete De;
        }
    }

    return 0;
}
