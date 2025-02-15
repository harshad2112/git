#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "../include/HashObject.hpp"
#include "../include/Utils.hpp"

bool hashObject(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Invalid arguments, required a flag and blob-sha.\n";
        return EXIT_FAILURE;
    }
    std::string flag = argv[2];
    std::string file;
    if (argc == 4)
    {
        file = argv[3];
    }
    else
    {
        file = flag;
        flag = "";
    }
    std::filesystem::path currentFolder = std::filesystem::current_path();
    std::filesystem::path filePath = currentFolder / file;
    auto commitFile = std::ifstream(filePath);
    if (!commitFile.is_open())
    {
        std::cerr << "fatal: Not a valid object name " << file << "\n";
        return EXIT_FAILURE;
    }
    std::vector<char> decompressed((std::istreambuf_iterator<char>(commitFile)), {});
    commitFile.close();

    std::string fileData(decompressed.begin(), decompressed.end());
    
    std::string type = "blob";
    std::cout<<writeObject(type, fileData, flag=="-w")<<'\n'; 

    return 0;
}
