#include <iostream>
#include <fstream>
#include <vector>
#include <string>

bool test() {
      //std::ifstream fileStream("/home/ysong/YSONG/ECALBeamTest/rawdata/savedata_20240705", std::ios::binary);
      
      //std::ifstream fileStream("filelist.txt", std::ios::binary);

  //string fileName="filelist.txt";
  string fileName="/home/ysong/YSONG/ECALBeamTest/rawdata/savedata_20240705";

  size_t lastSlashPos = fileName.find_last_of("/");
  if(lastSlashPos != std::string::npos) cout<< fileName.substr(lastSlashPos+1)<<endl;

   size_t dotPos = fileName.find_last_of('.');
       if (dotPos == std::string::npos) {return false; // 文件名中没有扩展名，不是文本文件
       }

std::string extension = fileName.substr(dotPos + 1);

if (extension == "txt" || extension == "csv" || extension == "xml") {

          return true; // 是文本文件
              } else {

                        return false; // 不是文本文件
                            }
 }
