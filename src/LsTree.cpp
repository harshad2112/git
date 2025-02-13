#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "../include/LsTree.hpp"
#include "../include/CatFile.hpp"
#include "../include/Utils.hpp"

void printOutput(std::string &flag, std::string &mode, std::string &type, std::string &data, std::string &siz, std::string &name)
{
    if(flag == "")
    {
        std::cout<<mode<<" "<<type<<" "<<data<<"    "<<name<<"\n";
    }
    else if(flag == "-l")
    {
        siz = std::string(58 - (siz.size() + mode.size() + type.size() + data.size()), ' ') + siz;
        std::cout<<mode<<" "<<type<<" "<<data<<" "<<siz<<"    "<<name<<"\n"; 
    }
}

bool lsTree(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Invalid arguments, required a flag and blob-sha.\n";
        return EXIT_FAILURE;
    }
    std::string flag = argv[2];
    std::string hash;
    if (argc == 4)
    {
        hash = argv[3];
    }
    else
    {
        hash = flag;
        flag = "";
    }
    std::string fileData = getFileData(hash);
    int nullDelimiter = fileData.find('\0');
    std::string header = fileData.substr(0, nullDelimiter);
    std::string data = fileData.substr(nullDelimiter);
    if(header.find("tree")==-1)
    {
        std::cout<<"Not a tree\n";
        return 0;
    }
    std::istringstream iss(data);
    while(iss)
    {
        std::string mode, name;
        std::getline(iss, mode, ' ');
        std::getline(iss, name, '\0');
        if(mode.size()<6)
        {
            mode = std::string(6 - mode.size(), '0') + mode;
        }
        char sha1Binary[20];
        iss.read(sha1Binary, 20);
        if(iss)
        {
            std::string hexData = computeHex(reinterpret_cast<unsigned char *> (sha1Binary));
            std::string objectData = getFileData(hexData);
            nullDelimiter = objectData.find('\0');
            header = objectData.substr(0, nullDelimiter);
            int spaceDelimiter = header.find(' ');
            std::string type = header.substr(0, spaceDelimiter);
            std::string objectSize = header.substr(spaceDelimiter);
            printOutput(flag, mode, type, hexData, objectSize, name);
        }
    } 
    return 0;
}
