#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "../include/Init.hpp"
#include "../include/HashObject.hpp"
#include "../include/CatFile.hpp"
#include "../include/LsTree.hpp"


int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    try
    {
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;
    
        if (argc < 2)
        {
            std::cerr << "No command provided.\n";
            return EXIT_FAILURE;
        }
    
        std::string command = argv[1];
    
        if (command == "init")
        {
            initializeGit(argc, argv);
        }
        else if (command == "cat-file")
        {
            catFile(argc, argv);
        }
        else if (command == "hash-object")
        {
            hashObject(argc, argv);
        }
        else if(command == "ls-tree")
        {
    	    lsTree(argc, argv);
        } 
        else
        {
            std::cerr << "Unknown command " << command << '\n';
            return EXIT_FAILURE;
        }
    
        return EXIT_SUCCESS;
    } 
    catch(const std::exception &err)
    {
        std::cerr<<err.what()<<'\n';
    }
}

