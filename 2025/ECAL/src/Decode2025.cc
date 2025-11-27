#include "Decode2025.hh"
#include <iostream>
#include <fstream>

int hex2float(unsigned short tmp)
{
    unsigned short Tmp = tmp;
    unsigned short tmpbit = 0;
    tmpbit = Tmp >> 14;
    Tmp = Tmp - tmpbit * 16384;
    Tmp = Tmp * 4;
    return Tmp;
}

// 电子学板通道号(0-5)对应到Hit列方向数
int Channel2Hit(int ch)
{
    int hit;
    switch (ch)
    {
    case 3:
        hit = 5;
        break;
    case 0:
        hit = 4;
        break;
    case 1:
        hit = 3;
        break;
    case 4:
        hit = 2;
        break;
    case 5:
        hit = 1;
        break;
    default:
        break;
    }
    return hit;
}

// 电子学板通道号对应到晶体阵列位置C
int Channel2C_invert(int ch)
{
    int C;
    switch (ch)
    {
    case 3:
        C = 36;
        break;
    case 0:
        C = 35;
        break;
    case 1:
        C = 34;
        break;
    case 4:
        C = 33;
        break;
    case 5:
        C = 32;
        break;
    default:
        break;
    }
    return C;
}
int Channel2C(int ch)
{
    int C;
    switch (ch)
    {
    case 3:
        C = 32;
        break;
    case 0:
        C = 33;
        break;
    case 1:
        C = 34;
        break;
    case 4:
        C = 35;
        break;
    case 5:
        C = 36;
        break;
    default:
        break;
    }
    return C;
}

void Decode2025::throw_error(std::string info)
{
    std::cerr << info << std::endl;
    exit(EXIT_FAILURE);
}

bool Decode2025::clear(float (&mLamp)[6][_Npoints], float (&mHamp)[6][_Npoints], float (&mLnoi)[6][_Nnoise], float (&mHnoi)[6][_Nnoise], float (&mLbase)[6], float (&mHbase)[6], float (&mLpeak)[6], float (&mHpeak)[6])
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < _Npoints; j++)
        {
            mLamp[i][j] = 0;
            mHamp[i][j] = 0;
            if (j < _Nnoise)
                mLnoi[i][j] = 0;
            if (j < _Nnoise)
                mHnoi[i][j] = 0;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        mLbase[i] = 0;
        mHbase[i] = 0;
        mLpeak[i] = 0;
        mHpeak[i] = 0;
    }

    return true;
}

bool Decode2025::ReadState(std::ifstream &indata, float (&temperature)[10], float &voltage, float &current)
{
    // move pointer to state information
    indata.seekg(16, indata.cur);
    float state;
    unsigned int *tmp = new unsigned int();
    for (int i = 0; i < 12; i++)
    {
        indata.read((char *)tmp, sizeof(unsigned short));
        state = hex2float(*tmp);
        if (i < 4)
            temperature[3 - i] = ((state / 65536 * 1.15) * 1000 - 943.227) / -5.194 + 30;
        else if (i < 8)
            temperature[11 - i] = ((state / 65536 * 1.15) * 1000 - 943.227) / -5.194 + 30;
        else if (i > 9)
            temperature[19 - i] = ((state / 65536 * 1.15) * 1000 - 943.227) / -5.194 + 30;

        if (i == 8)
            current = (state / 65536 * 1.15) / 2 * 50;
        if (i == 9)
            voltage = (state / 65536 * 1.15) / 2 * 800;
    }
    // move pointer to the head of block (pure data part)
    indata.seekg(-40, indata.cur);
    return true;
}

bool Decode2025::ReadDataDAQ(std::ifstream &indata, float (&mLamp)[6][_Npoints], float (&mHamp)[6][_Npoints], float (&mLnoi)[6][_Nnoise], float (&mHnoi)[6][_Nnoise], float (&mLbase)[6], float (&mHbase)[6], float (&mLpeak)[6], float (&mHpeak)[6])
{
    unsigned short *tmp = new unsigned short();
    unsigned short amp;
    int channel, point;
    bool HLG;
    // skip block header
    indata.seekg(24, indata.cur);
    // loop over 3 clusters
    for (int i = 0; i < 3; i++)
    {
        indata.read((char *)tmp, sizeof(unsigned short));
        if (static_cast<int>(*tmp) != _Npoints)
            throw_error("Defined sampled point number `_Npoints` is wrong!");
        indata.seekg(4, indata.cur);
        indata.read((char *)tmp, sizeof(unsigned short));
        if (static_cast<int>(*tmp) != i)
            throw_error("ADC serial number is not in order!");
        // loop over 4 channels
        for (int j = 0; j < 4 * _Npoints; j++)
        {
            channel = i * 2 + j % 4 / 2;
            HLG = static_cast<bool>((j + 1) % 2);
            point = j / 4;

            indata.read((char *)tmp, sizeof(unsigned short));
            amp = *tmp / 4;
            if (HLG)
            {
                mHamp[channel][point] = amp;
                if (point < _Nnoise)
                {
                    mHnoi[channel][point] = amp;
                    mHbase[channel] += amp;
                }
                if (point == 0)
                    mHpeak[channel] = amp;
                else if (amp > mHpeak[channel] && point > _timewinb && point < _timewine)
                    mHpeak[channel] = amp;
            }
            else
            {
                mLamp[channel][point] = amp;
                if (point < _Nnoise)
                {
                    mLnoi[channel][point] = amp;
                    mLbase[channel] += amp;
                }
                if (point == 0)
                    mLpeak[channel] = amp;
                else if (amp > mLpeak[channel] && point > _timewinb && point < _timewine)
                    mLpeak[channel] = amp;
            }
        }
    }
    // skip the 2nd long timecode
    for (int i = 0; i < 4; i++)
        indata.read((char *)tmp, sizeof(unsigned short));
    delete tmp;
    return true;
}

// read waveform points in one frame
bool Decode2025::ReadData(std::ifstream &indata, float (&mLamp)[6][_Npoints], float (&mHamp)[6][_Npoints], float (&mLnoi)[6][_Nnoise], float (&mHnoi)[6][_Nnoise], float (&mLbase)[6], float (&mHbase)[6], float (&mLpeak)[6], float (&mHpeak)[6])
{
    unsigned short *tmp = new unsigned short();
    unsigned short amp;
    int channel, point;
    bool HLG;
    // skip block header
    indata.seekg(48, indata.cur);
    // loop over 3 clusters
    for (int i = 0; i < 3; i++)
    {
        indata.read((char *)tmp, sizeof(unsigned short));
        if (static_cast<int>(*tmp) != _Npoints)
            throw_error("Defined sampled point number `_Npoints` is wrong!");
        indata.seekg(4, indata.cur);
        indata.read((char *)tmp, sizeof(unsigned short));
        if (static_cast<int>(*tmp) != i)
            throw_error("ADC serial number is not in order!");
        // loop over 4 channels
        for (int j = 0; j < 4 * _Npoints; j++)
        {
            channel = i * 2 + j % 4 / 2;
            HLG = static_cast<bool>((j + 1) % 2);
            point = j / 4;

            indata.read((char *)tmp, sizeof(unsigned short));
            amp = *tmp / 4;
            if (!HLG)
            {
                mHamp[channel][point] = amp;
                if (point < _Nnoise)
                {
                    mHnoi[channel][point] = amp;
                    mHbase[channel] += amp;
                }
                if (point == 0)
                    mHpeak[channel] = amp;
                else if (amp > mHpeak[channel] && point > _timewinb && point < _timewine)
                    mHpeak[channel] = amp;
            }
            else
            {
                mLamp[channel][point] = amp;
                if (point < _Nnoise)
                {
                    mLnoi[channel][point] = amp;
                    mLbase[channel] += amp;
                }
                if (point == 0)
                    mLpeak[channel] = amp;
                else if (amp > mLpeak[channel] && point > _timewinb && point < _timewine)
                    mLpeak[channel] = amp;
            }
        }
    }
    // skip the 2nd long timecode
    for (int i = 0; i < 4; i++)
        indata.read((char *)tmp, sizeof(unsigned short));
    delete tmp;
    return true;
}

// 读出一块板的在线处理信息
bool Decode2025::ReadData(std::ifstream &indata, double &timestamp, std::vector<unsigned short> &channelID, std::vector<double> &coarsetime, std::vector<double> &finetime, std::vector<double> &amplitude)
{
    unsigned short BoardChannel;
    unsigned long TQinfo;
    unsigned short *ClusterLength = new unsigned short();
    unsigned short *tmp = new unsigned short();
    int ClusterNum;
    int ch, board, ctime, ftime, amp;
    unsigned long temp1, temp2, temp3;

    indata.seekg(42, indata.cur);
    // get cluster length
    indata.read((char *)ClusterLength, sizeof(unsigned short));
    if (*ClusterLength % 8 != 0)
        throw_error("cluster length is wrong!");
    // get short timdecode
    indata.read((char *)tmp, sizeof(unsigned short));
    timestamp = *tmp * 12.5;
    // get cluster triggerID
    indata.read((char *)tmp, sizeof(unsigned short));
    if (*tmp != 0x0001)
        throw_error("cluster triggerID is not 1!");

    // read online fit result
    ClusterNum = (*ClusterLength - 8) / 8;
    for (int i = 0; i < ClusterNum; i++)
    {
        indata.read((char *)&TQinfo, 6);
        indata.read((char *)&BoardChannel, sizeof(unsigned short));
        board = BoardChannel >> 4;
        ch = BoardChannel - static_cast<int>(pow(2, 4)) * board;
        temp1 = TQinfo >> 19;
        amp = TQinfo - static_cast<int>(pow(2, 19)) * temp1;
        temp2 = temp1 >> 12;
        ftime = temp1 - static_cast<int>(pow(2, 12)) * temp2;
        temp3 = temp2 >> 16;
        ctime = temp2 - static_cast<int>(pow(2, 16)) * temp3;
        channelID.push_back(ch);
        coarsetime.push_back(ctime * 12.5);
        finetime.push_back(ftime * 12.5);
        amplitude.push_back(amp);
    }

    // skip the 2nd long timecode
    for (int i = 0; i < 4; i++)
        indata.read((char *)tmp, sizeof(unsigned short));
    delete tmp;
    delete ClusterLength;
    return true;
}

Decode2025::Decode2025(std::string filename) : mHit(25)
{
    for (int i = 0; i < 25; i++)
        mHit[i] = new DataModel2025();

    mFile = new TFile(filename.data(), "recreate");
    mTree = new TTree("decode_data", "decode_data");

    mTree->Branch("EventID", &mEventID, "EventID/L");
    mTree->Branch("TriggerID", &mTriggerID, "TriggerID/I");
    mTree->Branch("TimeCode", &mTimeCode, "TimeCode/L");
    mTree->Branch("Time", &mTime, Form("Time[%d]/F", _Npoints));
    for (int i = 0; i < 5; i++)
    {
        std::string name = "Voltage_" + std::to_string(i + 1);
        mTree->Branch(name.data(), &mVoltage[i], Form("Voltage%d/F", i));
    }

    for (int i = 0; i < 5; i++)
    {
        std::string name = "Current_" + std::to_string(i + 1);
        mTree->Branch(name.data(), &mCurrent[i], Form("Current%d/F", i));
    }

    std::string Name[25];
    for (int i = 0; i < 25; i++)
    {
        int no;
        if (i % 5 == 0)
            no = 4;
        if (i % 5 == 1)
            no = 3;
        if (i % 5 == 2)
            no = 5;
        if (i % 5 == 3)
            no = 2;
        if (i % 5 == 4)
            no = 1;
        Name[i] = std::to_string(i / 5 + 1) + "_" + std::to_string(no);
    }
    if (mChannelInvert)
    {
        for (int i = 0; i < 25; i++)
        {
            int no;
            if (i < 15)
            {
                if (i % 5 == 0)
                    no = 2;
                if (i % 5 == 1)
                    no = 3;
                if (i % 5 == 2)
                    no = 1;
                if (i % 5 == 3)
                    no = 4;
                if (i % 5 == 4)
                    no = 5;
            }
            else
                continue;
            Name[i] = std::to_string(i / 5 + 1) + "_" + std::to_string(no);
        }
    }
    for (int i = 0; i < 25; i++)
    {
        std::string name = "Hit_" + Name[i];
        std::string leaf_list = "";

        leaf_list += "CrystalID/L";
        leaf_list += ":Temperature1/D";
        leaf_list += ":Temperature2/D";
        leaf_list += Form(":LAmplitude[%d]/D", _Npoints);
        leaf_list += Form(":HAmplitude[%d]/D", _Npoints);
        leaf_list += Form(":LNoise[%d]/D", _Nnoise);
        leaf_list += Form(":HNoise[%d]/D", _Nnoise);
        leaf_list += ":LowGainPedestal/D";
        leaf_list += ":HighGainPedestal/D";
        leaf_list += ":LowGainPeak/D";
        leaf_list += ":HighGainPeak/D";

        mTree->Branch(name.data(), &mHit[i]->CrystalID, leaf_list.data());
        mTree->Branch(Form("%s_TimeStamp", name.data()), &mHit[i]->TimeStamp, "TimeStamp/D");
        mTree->Branch(Form("%s_CoarseTime", name.data()), &mHit[i]->CoarseTime);
        mTree->Branch(Form("%s_FineTime", name.data()), &mHit[i]->FineTime);
        mTree->Branch(Form("%s_Amplitude", name.data()), &mHit[i]->Amplitude);
    }

    mEventID = mTriggerID = mTimeCode = 0;
    memset(mTime, 0, sizeof(mTime));
    memset(mVoltage, 0, sizeof(mVoltage));
    memset(mCurrent, 0, sizeof(mCurrent));
    memset(mTemperature, 0, sizeof(mTemperature));
}

Decode2025::~Decode2025()
{
    if (mFile != nullptr)
    {
        mFile->cd();
        mTree->Write();
    }
    mFile->Close();
    delete mFile;
    mFile = nullptr;
}

void Decode2025::GetHit(std::ifstream &indata)
{
    // each event contains 5 frames/boards, each frame contains 1 block, each block contains 3 clusters/ADCs, each cluster contains 4 channels (2 H/LG ones)
    // amplitude, noise, baseline, peak
    float mLamp[6][_Npoints];
    float mHamp[6][_Npoints];
    float mLnoi[6][_Nnoise];
    float mHnoi[6][_Nnoise];
    float mLbase[6], mHbase[6], mLpeak[6], mHpeak[6];
    clear(mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);

    unsigned short *tmp = new unsigned short();
    unsigned short *tmp1 = new unsigned short();
    unsigned short *tmp2 = new unsigned short();
    unsigned short *FrameLength = new unsigned short();
    unsigned short *TriggerID = new unsigned short();
    unsigned short *BoardID = new unsigned short();
    unsigned short *BlockNum = new unsigned short();
    unsigned short *timecodeS = new unsigned short();
    unsigned long *timecodeL1 = new unsigned long();
    unsigned long *timecodeL2 = new unsigned long();

    int CrystalID, M, C;

    indata.seekg(0, indata.end);
    long long int length = indata.tellg();
    indata.seekg(0, indata.beg);

    int progress = 0, lastprogress = 0;

    Long64_t Nevent = 0;
    // make sure the remaining file length > 5 frames
    while (!indata.eof() && indata.good() && ((length - indata.tellg()) >= 5 * (192 + _Npoints * 8)))
    {
        progress = static_cast<int>(100. * (static_cast<long long>(indata.tellg()) + 5 * (192 + _Npoints * 8)) / length);
        if (progress % 5 == 0 && progress != 0 && progress != lastprogress)
        {
            std::cout << "Progress: " << progress << "%\r" << std::endl;
            std::cout.flush();
            lastprogress = progress;
        }
        // loop over 5 frames
        // assume that triggerID increases in sequence, but boardID may be out of order
        for (int nb = 0; nb < 5; nb++)
        {
            // read frame header(0x8303),length of frame, trigger ID, board ID, block number, short timecode
            indata.read((char *)tmp, sizeof(unsigned short));
            indata.read((char *)FrameLength, sizeof(unsigned short));
            indata.read((char *)TriggerID, sizeof(unsigned short));
            indata.read((char *)BoardID, sizeof(unsigned short));
            indata.read((char *)tmp1, sizeof(unsigned short));
            indata.read((char *)tmp2, sizeof(unsigned short));
            indata.read((char *)BlockNum, sizeof(unsigned short));
            indata.read((char *)timecodeS, sizeof(unsigned short));
            // check frame header
            if (*tmp == 0x8303 && *tmp1 == 0xeb01 && *tmp2 == 0x55aa)
            {
                // check block number
                if (*BlockNum != 1)
                    throw_error("Block number is not 1!");
                // check triggerID
                if (nb == 0)
                    mTriggerID = *TriggerID;
                else if (mTriggerID != *TriggerID)
                    throw_error("The triggerID in one event do not match!");
                // check long timecode
                indata.read((char *)timecodeL1, sizeof(unsigned long));
                indata.seekg(-24 + *FrameLength - 8, indata.cur);
                indata.read((char *)timecodeL2, sizeof(unsigned long));
                if (*timecodeL1 == *timecodeL2)
                {
                    if (nb == 0)
                        mTimeCode = *timecodeL1;
                    else if (mTimeCode != *timecodeL1)
                        throw_error("The timecodes in one event do not match!");
                }
                else
                    throw_error("Two timecodes in one frame do not match!");
                // ifstream pointer reset to the head of block
                indata.seekg(-static_cast<Long64_t>(*FrameLength) + 8, indata.cur);

                // boardID&channelID correspond to crystalID&hitID
                // assign central crystalID to be 326034
                // Y axes
                // ^
                // |
                // |
                // |boardID: 0   1   2   3   4
                // |  C:                        channelID:
                // |  032 # 1-5 2-5 3-5 4-5 5-5     3
                // |  033 # 1-4 2-4 3-4 4-4 5-4     0
                // |  034 # 1-3 2-3 3-3 4-3 5-3     1
                // |  035 # 1-2 2-2 3-2 4-2 5-2     4
                // |  036 # 1-1 2-1 3-1 4-1 5-1     5
                // |      # # # # # # # # # # #
                // |  M:    28  27  26  25  24
                // ----------------------------------> X axes

                // read states of a board: current, voltage and 10 temperature points
                ReadState(indata, mTemperature, mVoltage[*BoardID], mCurrent[*BoardID]);
                // read information of a board (3*2 H/LG channel): waveform, noise, baseline and peak
                clear(mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);
                ReadDataDAQ(indata, mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);

                // set information of Hits
                for (int i = 0; i < 6; i++)
                {
                    // ch3 is redundant
                    if (i == 2)
                        continue;
                    float ampLG[_Npoints], ampHG[_Npoints], noiLG[_Nnoise], noiHG[_Nnoise], temperature[2];
                    // baseline
                    mLbase[i] = mLbase[i] / _Nnoise;
                    mHbase[i] = mHbase[i] / _Nnoise;
                    // amplitude
                    for (int j = 0; j < _Npoints; j++)
                    {
                        ampLG[j] = mLamp[i][j];
                        ampHG[j] = mHamp[i][j];
                    }
                    // noise
                    for (int j = 0; j < _Nnoise; j++)
                    {
                        noiLG[j] = mLnoi[i][j];
                        noiHG[j] = mHnoi[i][j];
                    }
                    // temperature
                    if (i < 2)
                    {
                        for (int j = 0; j < 2; j++)
                            temperature[j] = mTemperature[j + 2 * i];
                    }
                    else
                    {
                        for (int j = 0; j < 2; j++)
                            temperature[j] = mTemperature[j + 2 * (i - 1)];
                    }
                    // crystalID
                    M = 28 - *BoardID;
                    if (i < 2)
                    {
                        C = 34 - i;
                    }
                    else
                    {
                        C = 40 - i;
                    }
                    CrystalID = 3 * 100000 + M * 1000 + C;

                    if (i < 2)
                        mHit[*BoardID * 5 + i]->Set(CrystalID, temperature[0], temperature[1], ampLG, ampHG, noiLG, noiHG, mLbase[i], mHbase[i], mLpeak[i], mHpeak[i]);
                    else
                        mHit[*BoardID * 5 + i - 1]->Set(CrystalID, temperature[0], temperature[1], ampLG, ampHG, noiLG, noiHG, mLbase[i], mHbase[i], mLpeak[i], mHpeak[i]);
                }
            }
            else
                throw_error("Frame header is wrong! Not equal to 0x8303.");
        }
        Nevent++;
        mEventID = Nevent;
        for (int i = 0; i < _Npoints; i++)
            mTime[i] = 12.5 * i;
        mTree->Fill();
    }

    delete tmp;
    delete tmp1;
    delete tmp2;
    delete FrameLength;
    delete TriggerID;
    delete BoardID;
    delete BlockNum;
    delete timecodeS;
    delete timecodeL1;
    delete timecodeL2;

    std::cout << "Length of the file is : " << length << "\tNumber of events is: " << Nevent << std::endl;
}

void Decode2025::GetHitDAQ(std::ifstream &indata)
{
    // each event contains a frame, each frame contains 5 block, each block contains 3 clusters/ADCs, each cluster contains 4 channels (2 H/LG ones)
    // amplitude, noise, baseline, peak
    float mLamp[6][_Npoints];
    float mHamp[6][_Npoints];
    float mLnoi[6][_Nnoise];
    float mHnoi[6][_Nnoise];
    float mLbase[6], mHbase[6], mLpeak[6], mHpeak[6];
    clear(mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);

    unsigned short *tmp = new unsigned short();
    unsigned short *tmp1 = new unsigned short();
    unsigned long *tmpL = new unsigned long();
    unsigned long *TriggerID = new unsigned long();
    unsigned long *ShiftAddress = new unsigned long();
    unsigned int *FrameLength = new unsigned int();
    unsigned short *BlockNum = new unsigned short();
    unsigned int *BlockLength = new unsigned int();
    unsigned short *BoardID = new unsigned short();
    unsigned long *timecodeL1 = new unsigned long();
    unsigned long *timecodeL2 = new unsigned long();
    unsigned short *timecodeS = new unsigned short();

    int CrystalID, M, C;

    indata.seekg(0, indata.end);
    long long int length = indata.tellg();
    indata.seekg(0, indata.beg);
    // read total number of frames
    indata.read((char *)tmp, sizeof(unsigned short));
    const unsigned short FrameNum = *tmp;

    int progress = 0, lastprogress = 0;
    // loop over all events
    for (Long64_t Nevent = 0; (Nevent < FrameNum) && !indata.eof() && indata.good(); Nevent++)
    {
        progress = static_cast<int>(100. * (Nevent + 1) / FrameNum);
        if (progress % 5 == 0 && progress != 0 && progress != lastprogress)
        {
            std::cout << "Progress: " << progress << "%\r" << std::endl;
            std::cout.flush();
            lastprogress = progress;
        }
        // move ifstream pointer to address index
        indata.seekg(2 + Nevent * (6 + 8), indata.beg);
        // read triggerID and shift address
        *TriggerID = 0;
        indata.read((char *)TriggerID, 6);
        mTriggerID = *TriggerID;
        indata.read((char *)ShiftAddress, sizeof(unsigned long));
        // move pointer to frame head
        indata.seekg(*ShiftAddress, indata.beg);
        // read frame head, tail and length
        indata.read((char *)tmp, sizeof(unsigned short));
        indata.read((char *)FrameLength, sizeof(unsigned int));
        // check triggerID and block number
        *tmpL = 0, *BlockNum = 0;
        indata.read((char *)tmpL, 6);
        if (*tmpL != *TriggerID)
            // throw_error("The triggerID in one event do not match!");
            std::cout << "Warning: The triggerID in one event do not match!" << std::endl;
        indata.read((char *)BlockNum, 1);
        if (*BlockNum != 5)
            // throw_error("Block number is not 5!");
            std::cout << "Warning: Block number is not 5!" << std::endl;
        indata.seekg(-13 + *FrameLength - 2, indata.cur);
        indata.read((char *)tmp1, sizeof(unsigned short));
        // check frame head&tail
        if (*tmp == 0x8013 && *tmp1 == 0xC000)
        {
            // move pointer to block head
            indata.seekg(-static_cast<Long64_t>(*FrameLength) + 13, indata.cur);
            // loop over 5 blocks (from 0 to 4)
            for (int nb = 0; nb < 5; nb++)
            {
                // check boardID
                indata.read((char *)BoardID, sizeof(unsigned short));
                if (*BoardID != nb)
                    // throw_error("BoardID is not in order!");
                    std::cout << "Warning: BoardID is not in order!" << std::endl;
                indata.read((char *)BlockLength, sizeof(unsigned int));
                indata.read((char *)tmp, sizeof(unsigned short));
                indata.read((char *)tmp1, sizeof(unsigned short));
                indata.read((char *)BlockNum, sizeof(unsigned short));
                indata.read((char *)timecodeS, sizeof(unsigned short));
                // check block (pure data part) head
                if (*tmp == 0xeb01 && *tmp1 == 0x55aa)
                {
                    // check block number
                    if (*BlockNum != 1)
                        // throw_error("Block number is not 1!");
                        std::cout << "Warning: Block number is not 1!" << std::endl;
                    // check long timecode
                    indata.read((char *)timecodeL1, sizeof(unsigned long));
                    indata.seekg(-22 + *BlockLength - 8, indata.cur);
                    indata.read((char *)timecodeL2, sizeof(unsigned long));
                    if (*timecodeL1 == *timecodeL2)
                    {
                        if (nb == 0)
                            mTimeCode = *timecodeL1;
                        else if (mTimeCode != *timecodeL1)
                            // throw_error("The timecodes in one event do not match!");
                            std::cout << "Warning: The timecodes in one event do not match!" << std::endl;
                    }
                    else
                        // throw_error("Two timecodes in one block do not match!");
                        std::cout << "Warning: Two timecodes in one block do not match!" << std::endl;
                    // move pointer to the head of block (pure data part)
                    indata.seekg(-static_cast<Long64_t>(*BlockLength) + 6, indata.cur);

                    // boardID&channelID correspond to crystalID&hitID
                    // assign central crystalID to be 326034
                    // Y axes
                    // ^
                    // |
                    // |
                    // |boardID: 0   1   2   3   4
                    // |  C:                        channelID:
                    // |  032 # 1-5 2-5 3-5 4-5 5-5     3
                    // |  033 # 1-4 2-4 3-4 4-4 5-4     0
                    // |  034 # 1-3 2-3 3-3 4-3 5-3     1
                    // |  035 # 1-2 2-2 3-2 4-2 5-2     4
                    // |  036 # 1-1 2-1 3-1 4-1 5-1     5
                    // |      # # # # # # # # # # #
                    // |  M:    28  27  26  25  24
                    // ----------------------------------> X axes

                    // read states of a board: current, voltage and 10 temperature points
                    ReadState(indata, mTemperature, mVoltage[*BoardID], mCurrent[*BoardID]);
                    // read information of a board (3*2 H/LG channel): waveform, noise, baseline and peak
                    clear(mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);
                    ReadDataDAQ(indata, mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);

                    // set information of Hits
                    for (int i = 0; i < 6; i++)
                    {
                        // ch3 is redundant
                        if (i == 2)
                            continue;
                        float ampLG[_Npoints], ampHG[_Npoints], noiLG[_Nnoise], noiHG[_Nnoise], temperature[2];
                        // baseline
                        mLbase[i] = mLbase[i] / _Nnoise;
                        mHbase[i] = mHbase[i] / _Nnoise;
                        // amplitude
                        for (int j = 0; j < _Npoints; j++)
                        {
                            ampLG[j] = mLamp[i][j];
                            ampHG[j] = mHamp[i][j];
                        }
                        // noise
                        for (int j = 0; j < _Nnoise; j++)
                        {
                            noiLG[j] = mLnoi[i][j];
                            noiHG[j] = mHnoi[i][j];
                        }
                        // temperature
                        if (i < 2)
                        {
                            for (int j = 0; j < 2; j++)
                                temperature[j] = mTemperature[j + 2 * i];
                        }
                        else
                        {
                            for (int j = 0; j < 2; j++)
                                temperature[j] = mTemperature[j + 2 * (i - 1)];
                        }
                        // crystalID
                        M = 28 - *BoardID;
                        switch (i)
                        {
                        case 3:
                            C = 32;
                            break;
                        case 0:
                            C = 33;
                            break;
                        case 1:
                            C = 34;
                            break;
                        case 4:
                            C = 35;
                            break;
                        case 5:
                            C = 36;
                            break;
                        default:
                            break;
                        }
                        CrystalID = 3 * 100000 + M * 1000 + C;

                        if (i < 2)
                            mHit[*BoardID * 5 + i]->Set(CrystalID, temperature[0], temperature[1], ampLG, ampHG, noiLG, noiHG, mLbase[i], mHbase[i], mLpeak[i], mHpeak[i]);
                        else
                            mHit[*BoardID * 5 + i - 1]->Set(CrystalID, temperature[0], temperature[1], ampLG, ampHG, noiLG, noiHG, mLbase[i], mHbase[i], mLpeak[i], mHpeak[i]);
                    }
                }
                else
                    throw_error("Block (pure data part) head is wrong");
            }
        }
        else
            throw_error("Frame head/tail is wrong! Not equal to 0x8013/0xC000.");

        mEventID++;
        for (int i = 0; i < _Npoints; i++)
            mTime[i] = 12.5 * i;
        mTree->Fill();
    }
    if (mEventID != FrameNum)
        // throw_error("Event number is less than frame index!");
        std::cout << "Warning: Event number is less than frame index!" << std::endl;

    delete tmp;
    delete tmp1;
    delete tmpL;
    delete TriggerID;
    delete ShiftAddress;
    delete FrameLength;
    delete BlockNum;
    delete BlockLength;
    delete BoardID;
    delete timecodeL1;
    delete timecodeL2;
    delete timecodeS;

    std::cout << "Length of the file is : " << length << "\tNumber of events is: " << mEventID << std::endl;
}

void Decode2025::GetHitOnline(std::ifstream &indata)
{
    // each event contains a frame, each frame contains 5 block, each block contains 3 clusters/ADCs, each cluster contains 4 channels (2 H/LG ones)
    // amplitude, noise, baseline, peak
    float mLamp[6][_Npoints];
    float mHamp[6][_Npoints];
    float mLnoi[6][_Nnoise];
    float mHnoi[6][_Nnoise];
    float mLbase[6], mHbase[6], mLpeak[6], mHpeak[6];
    clear(mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);
    double mTimestamp;
    std::vector<unsigned short> channelID_fill;
    std::vector<double> coarsetime_fill, finetime_fill, amplitude_fill;

    unsigned short *tmp = new unsigned short();
    unsigned short *tmp1 = new unsigned short();
    unsigned long *tmpL = new unsigned long();
    unsigned long *TriggerID = new unsigned long();
    unsigned long *ShiftAddress = new unsigned long();
    unsigned int *FrameLength = new unsigned int();
    unsigned short *BlockNum = new unsigned short();
    unsigned int *BlockLength = new unsigned int();
    unsigned short *BoardID = new unsigned short();
    unsigned long *timecodeL1 = new unsigned long();
    unsigned long *timecodeL2 = new unsigned long();
    unsigned short *timecodeS = new unsigned short();

    int CrystalID, M, C;

    indata.seekg(0, indata.end);
    long long int length = indata.tellg();
    indata.seekg(0, indata.beg);
    // read total number of frames
    indata.read((char *)tmp, sizeof(unsigned short));
    const unsigned short FrameNum = *tmp;

    int progress = 0, lastprogress = 0;
    // loop over all events
    for (Long64_t Nevent = 0; (Nevent < FrameNum) && !indata.eof() && indata.good(); Nevent++)
    {
        progress = static_cast<int>(100. * (Nevent + 1) / FrameNum);
        if (progress % 5 == 0 && progress != 0 && progress != lastprogress)
        {
            std::cout << "Progress: " << progress << "%\r" << std::endl;
            std::cout.flush();
            lastprogress = progress;
        }
        // move ifstream pointer to address index
        indata.seekg(2 + Nevent * (6 + 8), indata.beg);
        // read triggerID and shift address
        *TriggerID = 0;
        indata.read((char *)TriggerID, 6);
        mTriggerID = *TriggerID;
        indata.read((char *)ShiftAddress, sizeof(unsigned long));
        // move pointer to frame head
        indata.seekg(*ShiftAddress, indata.beg);
        // read frame head, tail and length
        indata.read((char *)tmp, sizeof(unsigned short));
        indata.read((char *)FrameLength, sizeof(unsigned int));
        // check triggerID and block number
        *tmpL = 0, *BlockNum = 0;
        indata.read((char *)tmpL, 6);
        if (*tmpL != *TriggerID)
            // throw_error("The triggerID in one event do not match!");
            std::cout << "Warning: The triggerID in one event do not match!" << std::endl;
        indata.read((char *)BlockNum, 1);
        bool nouse_online;
        if (*BlockNum == 5)
            nouse_online = true;
        else
            nouse_online = false;
        indata.seekg(-13 + *FrameLength - 2, indata.cur);
        indata.read((char *)tmp1, sizeof(unsigned short));
        // check frame head&tail
        if (*tmp == 0x8013 && *tmp1 == 0xC000)
        {
            // move pointer to block head
            indata.seekg(-static_cast<Long64_t>(*FrameLength) + 13, indata.cur);
            // loop over all blocks (first 5: waveform data; last 5: online processed data)
            for (int nb = 0; nb < 10; nb++)
            {
                // if no online information used, skip last 5 block
                if (nb >= 5 && nouse_online)
                    continue;
                // check boardID
                indata.read((char *)BoardID, sizeof(unsigned short));
                if (*BoardID != nb)
                    // throw_error("BoardID is not in order!");
                    std::cout << "Warning: BoardID is not in order!" << std::endl;
                indata.read((char *)BlockLength, sizeof(unsigned int));
                indata.read((char *)tmp, sizeof(unsigned short));
                indata.read((char *)tmp1, sizeof(unsigned short));
                indata.read((char *)BlockNum, sizeof(unsigned short));
                indata.read((char *)timecodeS, sizeof(unsigned short));
                // check block (pure data part) head
                if (*tmp == 0xeb01 && *tmp1 == 0x55aa)
                {
                    // check block number
                    if (*BlockNum != 1)
                        // throw_error("Block number is not 1!");
                        std::cout << "Warning: Block number is not 1!" << std::endl;
                    // check long timecode
                    indata.read((char *)timecodeL1, sizeof(unsigned long));
                    indata.seekg(-22 + *BlockLength - 8, indata.cur);
                    indata.read((char *)timecodeL2, sizeof(unsigned long));
                    if (*timecodeL1 == *timecodeL2)
                    {
                        if (nb == 0)
                            mTimeCode = *timecodeL1;
                        else if (mTimeCode != *timecodeL1)
                            // throw_error("The timecodes in one event do not match!");
                            std::cout << "Warning: The timecodes in one event do not match!" << std::endl;
                    }
                    else
                        // throw_error("Two timecodes in one block do not match!");
                        std::cout << "Warning: Two timecodes in one block do not match!" << std::endl;
                    // move pointer to the head of block (pure data part)
                    indata.seekg(-static_cast<Long64_t>(*BlockLength) + 6, indata.cur);
                    // 在线提取通道编号不同 1-6->
                    // boardID&channelID correspond to crystalID&hitID
                    // assign central crystalID to be 326034
                    // Y axes
                    // ^
                    // |
                    // |
                    // |boardID: 0   1   2   3   4
                    // |  C:                        channelID:
                    // |  032 # 1-5 2-5 3-5 4-5 5-5     3 3
                    // |  033 # 1-4 2-4 3-4 4-4 5-4     0 2
                    // |  034 # 1-3 2-3 3-3 4-3 5-3     1 1
                    // |  035 # 1-2 2-2 3-2 4-2 5-2     4 6
                    // |  036 # 1-1 2-1 3-1 4-1 5-1     5 5
                    // |      # # # # # # # # # # #
                    // |  M:    28  27  26  25  24
                    // ----------------------------------> X axes

                    // read states of a board: current, voltage and 10 temperature points
                    ReadState(indata, mTemperature, mVoltage[*BoardID], mCurrent[*BoardID]);
                    if (*BoardID < 5)
                    {
                        // read information of a board (3*2 H/LG channel): waveform, noise, baseline and peak
                        clear(mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);
                        ReadData(indata, mLamp, mHamp, mLnoi, mHnoi, mLbase, mHbase, mLpeak, mHpeak);
                        // set information of Hits
                        for (int i = 0; i < 6; i++)
                        {
                            // ch3 is redundant
                            if (i == 2)
                                continue;
                            float ampLG[_Npoints], ampHG[_Npoints], noiLG[_Nnoise], noiHG[_Nnoise], temperature[2];
                            // baseline
                            mLbase[i] = mLbase[i] / _Nnoise;
                            mHbase[i] = mHbase[i] / _Nnoise;
                            // amplitude
                            for (int j = 0; j < _Npoints; j++)
                            {
                                ampLG[j] = mLamp[i][j];
                                ampHG[j] = mHamp[i][j];
                            }
                            // noise
                            for (int j = 0; j < _Nnoise; j++)
                            {
                                noiLG[j] = mLnoi[i][j];
                                noiHG[j] = mHnoi[i][j];
                            }
                            // temperature
                            if (i < 2)
                            {
                                for (int j = 0; j < 2; j++)
                                    temperature[j] = mTemperature[j + 2 * i];
                            }
                            else
                            {
                                for (int j = 0; j < 2; j++)
                                    temperature[j] = mTemperature[j + 2 * (i - 1)];
                            }
                            // crystalID
                            M = 28 - *BoardID;
                            C = Channel2C(i);
                            if (mChannelInvert)
                            {
                                if (*BoardID <= 2)
                                    C = Channel2C_invert(i);
                            }
                            CrystalID = 3 * 100000 + M * 1000 + C;
                            switch (i)
                            {
                            case 4:
                                temperature[0] = mTemperature[2];
                                temperature[1] = mTemperature[3];
                                break;
                            case 1:
                                temperature[0] = mTemperature[0];
                                temperature[1] = mTemperature[1];
                                break;
                            case 5:
                                temperature[0] = mTemperature[4];
                                temperature[1] = mTemperature[5];
                                break;
                            case 0:
                                temperature[0] = mTemperature[8];
                                temperature[1] = mTemperature[9];
                                break;
                            case 3:
                                temperature[0] = mTemperature[6];
                                temperature[1] = mTemperature[7];
                                break;
                            default:
                                break;
                            }
                            if (mChannelInvert)
                            {

                                if (*BoardID <= 2)
                                {
                                    switch (i)
                                    {
                                    case 0:
                                        temperature[0] = mTemperature[2];
                                        temperature[1] = mTemperature[3];
                                        break;
                                    case 1:
                                        temperature[0] = mTemperature[0];
                                        temperature[1] = mTemperature[1];
                                        break;
                                    case 3:
                                        temperature[0] = mTemperature[4];
                                        temperature[1] = mTemperature[5];
                                        break;
                                    case 4:
                                        temperature[0] = mTemperature[8];
                                        temperature[1] = mTemperature[9];
                                        break;
                                    case 5:
                                        temperature[0] = mTemperature[6];
                                        temperature[1] = mTemperature[7];
                                        break;
                                    default:
                                        break;
                                    }
                                }
                            }

                            if (i < 2)
                                mHit[*BoardID * 5 + i]->Set(CrystalID, temperature[0], temperature[1], ampLG, ampHG, noiLG, noiHG, mLbase[i], mHbase[i], mLpeak[i], mHpeak[i]);
                            else
                                mHit[*BoardID * 5 + i - 1]->Set(CrystalID, temperature[0], temperature[1], ampLG, ampHG, noiLG, noiHG, mLbase[i], mHbase[i], mLpeak[i], mHpeak[i]);
                        }
                    }
                    else
                    {
                        channelID_fill.clear();
                        coarsetime_fill.clear();
                        finetime_fill.clear();
                        amplitude_fill.clear();
                        ReadData(indata, mTimestamp, channelID_fill, coarsetime_fill, finetime_fill, amplitude_fill);
                        for (size_t i = 0; i < channelID_fill.size(); i++)
                        {
                            int index, channel;
                            channel = channelID_fill.at(i);
                            switch (channel)
                            {
                            case 1:
                                index = 1;
                                break;
                            case 2:
                                index = 0;
                                break;
                            case 3:
                                index = 2;
                                break;
                            case 5:
                                index = 4;
                                break;
                            case 6:
                                index = 3;
                                break;
                            default:
                                throw_error("channel4 signal appears!");
                                break;
                            }
                            index += (*BoardID - 5) * 5;
                            mHit[index]->AddHit(mTimestamp, coarsetime_fill.at(i), finetime_fill.at(i), amplitude_fill.at(i));
                        }
                    }
                }
                else
                    throw_error("Block (pure data part) head is wrong");
            }
        }
        else
            throw_error("Frame head/tail is wrong! Not equal to 0x8013/0xC000.");

        mEventID++;
        for (int i = 0; i < _Npoints; i++)
            mTime[i] = 12.5 * i;
        mTree->Fill();
        for (int i = 0; i < 25; i++)
            mHit[i]->clear();
    }
    if (mEventID != FrameNum)
        // throw_error("Event number is less than frame index!");
        std::cout << "Warning: Event number is less than frame index!" << std::endl;

    delete tmp;
    delete tmp1;
    delete tmpL;
    delete TriggerID;
    delete ShiftAddress;
    delete FrameLength;
    delete BlockNum;
    delete BlockLength;
    delete BoardID;
    delete timecodeL1;
    delete timecodeL2;
    delete timecodeS;

    std::cout << "Length of the file is : " << length << "\tNumber of events is: " << mEventID << std::endl;
}
