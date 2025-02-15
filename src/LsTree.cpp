#include <string>
#include <iostream>

#include "../include/LsTree.hpp"
#include "../include/GitUtils.hpp"

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
    extractHash(hash, flag);    
    return 0;
}
