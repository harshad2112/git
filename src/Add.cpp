#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

#include "../include/Add.hpp"
#include "../include/IndexEntry.hpp"
#include "../include/GitUtils.hpp"

bool add(int argc, char* argv[])
{
    std::filesystem::path currentFolder = std::filesystem::current_path();
    std::filesystem::path filePath = currentFolder / ".git/index";
    std::string indexPath = filePath.string();
    std::string currentFolderPath = currentFolder.string();
    std::vector<IndexEntry> currentFiles = readGitIndex(indexPath);  
    std::vector<IndexEntry> dirFiles = getAllFiles(currentFolderPath);
    
}   


