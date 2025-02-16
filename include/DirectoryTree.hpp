#ifndef DIR_TREE_H
#define DIR_TREE_H

#include <vector>
#include <string>

#include "IndexEntry.hpp"

class DirectoryTree
{
    std::string name;
    std::string filePath;
    std::string type;
    std::string mode;
    std::vector<DirectoryTree* > child; 
    std::string sha;
    void createDirTree(DirectoryTree* root, std::vector<IndexEntry> entries);
    void createSHA(DirectoryTree* root);
public:
    DirectoryTree(const std::string &name, const std::string &type, const std::string &mode, const std::string &sha);
    DirectoryTree(std::vector<IndexEntry> entries);
    std::string getName();
    std::string getFilePath();
    void setFilePath(std::string fp);
    std::string getType();
    std::string getSHA();
    std::string getMode();
    void setMode(std::string mod);
    void setSHA(std::string sh);
    std::vector<DirectoryTree*> getChild();
};

#endif
