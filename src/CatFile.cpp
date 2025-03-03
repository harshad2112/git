#include <string>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "CatFile.hpp"
#include "Utils.hpp"
#include "GitUtils.hpp"

bool catFile(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Invalid arguments, required a flag and blob-sha.\n";
        return EXIT_FAILURE;
    }
    std::string flag = argv[2];
    std::string value = argv[3];
    if (flag == "-p")
    {
        std::string commitData = getFileData(value); 
        int delimiter = commitData.find('\0'); 
        std::string header = commitData.substr(0, delimiter);
        if(header.find("tree")!=-1)
        {
            extractHash(value, "");
            return 0;
        }
        std::cout<<commitData.substr(delimiter)<<"\n";
    }
    return 0;
}

