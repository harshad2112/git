#include <filesystem>
#include <iostream>
#include <vector>
#include <string>

#include "../include/WriteTree.hpp"
#include "../include/GitUtils.hpp"
#include "../include/DirectoryTree.hpp"

bool writeTree(int argc, char* argv[])
{
    
    std::filesystem::path currentFolder = std::filesystem::current_path();
    std::filesystem::path filePath = currentFolder / ".git/index";
    std::string indexPath = filePath.string();
    std::vector<IndexEntry> entries = readGitIndex(indexPath);  
    DirectoryTree *dirTree = new DirectoryTree(entries);
    std::cout<<dirTree->getSHA()<<'\n';
    return 0;
}
