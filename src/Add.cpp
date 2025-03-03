#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Add.hpp"
#include "IndexEntry.hpp"
#include "GitUtils.hpp"

bool add(int argc, char* argv[])
{
    std::filesystem::path currentFolder = std::filesystem::current_path();
    std::filesystem::path filePath = currentFolder / ".git/index";
    std::string indexPath = filePath.string();
    std::string currentFolderPath = currentFolder.string();
    std::vector<IndexEntry> currentFiles = readGitIndex(indexPath);  
    std::vector<IndexEntry> dirFiles = getAllFiles(currentFolderPath);
    for(auto dirFile: dirFiles)
    {
        std::ifstream file(currentFolder / dirFile.path, std::ios::binary);
        if(!file.is_open())
        {
            throw std::runtime_error("fatal: Not a valid object name " + dirFile.path);
        }
        std::vector<char> dataVec((std::istreambuf_iterator<char>(file)), {});
        file.close();
        std::string data(dataVec.begin(), dataVec.end());
        std::string sha1 = writeObject("blob", data, "false");
        dirFile.sha1 = sha1;
        bool alreadyPresent = false;
        for(auto &currentFile: currentFiles)
        {
            if(currentFile.path == dirFile.path)
            {   
                if(currentFile.sha1 != dirFile.sha1)
                {
                    currentFile = dirFile;
                }
                alreadyPresent = true;
                break;
            }
        }
        if(!alreadyPresent)
        {
            currentFiles.push_back(dirFile);
        }
    }
    writeGitIndex(currentFiles, currentFolder.string());
}   


