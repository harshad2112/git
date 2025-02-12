#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "../include/CatFile.hpp"
#include "../include/Utils.hpp"

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
        std::string folder = value.substr(0, 2);
        std::string file = value.substr(2);
        std::filesystem::path currentFolder = std::filesystem::current_path();
        std::filesystem::path filePath = currentFolder / ".git/objects/" / folder / file;
        auto commitFile = std::ifstream(filePath);
        if (!commitFile.is_open())
        {
            std::cerr << "fatal: Not a valid object name " << folder << file << "\n";
            return EXIT_FAILURE;
        }
        std::vector<char> compressed((std::istreambuf_iterator<char>(commitFile)), {});
        commitFile.close();

        std::vector<char> decompressed;
        if (decompressData(compressed, decompressed))
        {
            std::string data(decompressed.begin(), decompressed.end());
            int delimiterPos = data.find('\0');
            std::cout << data.substr(delimiterPos + 1);
        }
        else
        {
            std::cerr << "fatal: Not a valid object name " << folder << file << "\n";
            return EXIT_FAILURE;
        }
    }
    return 0;
}

