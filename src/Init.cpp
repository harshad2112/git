#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>

#include "../include/Init.hpp"

bool initializeGit(int argc, char *argv[])
{
    try
    {
        std::filesystem::create_directory(".git");
        std::filesystem::create_directory(".git/objects");
        std::filesystem::create_directory(".git/refs");

        std::ofstream headFile(".git/HEAD");
        if (headFile.is_open())
        {
            headFile << "ref: refs/heads/main\n";
            headFile.close();
        }
        else
        {
            std::cerr << "Failed to create .git/HEAD file.\n";
            return EXIT_FAILURE;
        }

        std::cout << "Initialized git directory\n";
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}
