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
    std::string dataToCompress = "blob " + std::to_string(fileData.size()) + '\0' + fileData;
    std::string shaString = computeSHA(dataToCompress);
    std::cout << shaString << "\n";

    if (flag == "-w")
    {
        std::vector<char> compressed;
        std::vector<char> input(dataToCompress.begin(), dataToCompress.end());
        if (compressData(input, compressed))
        {
            std::string data(compressed.begin(), compressed.end());
            std::string folder = shaString.substr(0, 2);
            std::string file = shaString.substr(2);
            std::filesystem::path folderPath = currentFolder / ".git/objects/" / folder;
            if (!std::filesystem::exists(folderPath))
            {
                std::filesystem::create_directories(folderPath);
            }
            filePath = currentFolder / ".git/objects/" / folder / file;
            auto commitFile = std::ofstream(filePath);
            if (!commitFile.is_open())
            {
                std::cerr << "fatal: Not able to write to file " << folder << file << "\n";
                return EXIT_FAILURE;
            }
            commitFile << data;
            commitFile.close();
        }
        else
        {
            std::cerr << "fatal: Not a valid object name " << file << "\n";
            return EXIT_FAILURE;
        }
    }
    return 0;
}
