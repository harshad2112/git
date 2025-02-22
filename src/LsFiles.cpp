#include <vector>
#include <filesystem>
#include <string>
#include <iostream>

#include "../include/LsFiles.hpp"
#include "../include/GitUtils.hpp"

void printLsFiles(int argc, char* argv[], std::vector<IndexEntry> &entries)
{
    if(argc == 2)
    {
        for(auto entry: entries)
        {
            std::cout<<entry.path<<'\n';
        }
    }
}

bool lsFiles(int argc, char* agrv[])
{
    std::filesystem::path currentFolder = std::filesystem::current_path();
    std::filesystem::path filePath = currentFolder / ".git/index";
    std::string indexPath = filePath.string();
    std::vector<IndexEntry> entries = readGitIndex(indexPath);  
    
    for(auto entry: entries)
    {
        std::cout<<entry.sha1<<" "<<entry.mode<<" "<<entry.path<<'\n';
    }
    
    return 0;
}
