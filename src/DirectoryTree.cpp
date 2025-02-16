#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

#include "../include/DirectoryTree.hpp"
#include "../include/GitUtils.hpp"
#include "../include/Utils.hpp"
#include "../include/IndexEntry.hpp"

bool sortOnFileName(DirectoryTree *a, DirectoryTree *b)
{
    return a->getName() < b->getName();
} 

DirectoryTree::DirectoryTree(const std::string &name, const std::string &type, const std::string &mode, const std::string &sha)
    : name(name), type(type), mode(mode), sha(sha) {}

DirectoryTree::DirectoryTree(std::vector<IndexEntry> entries)
{
    this->name = "root";
    this->type = "tree";
    this->filePath = "./";
    createDirTree(this, entries);
    createSHA(this);
}

std::string DirectoryTree::getName()
{
    return name;
}

std::string DirectoryTree::getType()
{
    return type;
}

std::vector<DirectoryTree* > DirectoryTree::getChild()
{
    return child;
}

std::string DirectoryTree::getSHA()
{
    return sha;
}

std::string DirectoryTree::getFilePath()
{
    return filePath;
}

void DirectoryTree::setFilePath(std::string fp)
{
    filePath = fp;
}

void DirectoryTree::setSHA(std::string sh)
{
    sha = sh;
}

std::string DirectoryTree::getMode()
{
    return mode;
}

void DirectoryTree::setMode(std::string mod)
{
    mode = mod;
}

void DirectoryTree::createDirTree(DirectoryTree* root, std::vector<IndexEntry> entries)
{
    std::map<std::string, DirectoryTree*> trees;
    std::map<std::string, std::vector<IndexEntry>> subTree;
    std::vector<DirectoryTree*> dirTree;
    for(auto entry: entries)
    {
        int pos = entry.path.find('/');
        if(pos!=-1)
        {
            std::string folderName = entry.path.substr(0, pos);
            if(!trees.count(folderName))
            {
                DirectoryTree* currTree = new DirectoryTree(folderName, "tree", "040000", entry.sha1);
                currTree->setFilePath(root->getFilePath() + '/' + folderName);
                trees[folderName] = currTree;
                dirTree.push_back(currTree);
            }
            IndexEntry childEntry = entry;
            childEntry.path = childEntry.path.substr(pos+1);
            subTree[folderName].push_back(childEntry);
        }
        else
        {
            DirectoryTree* currTree = new DirectoryTree(entry.path, "blob", decToOct(entry.mode), entry.sha1);
            currTree->setFilePath(root->getFilePath());
            dirTree.push_back(currTree);
            root->child.push_back(currTree);
        }
    }
    for(auto tree: trees)
    {
        
        createDirTree(tree.second, subTree[tree.first]);
        root->child.push_back(tree.second);
    }
    sort(root->child.begin(), root->child.end(), sortOnFileName);
    createSHA(root);
}

void DirectoryTree::createSHA(DirectoryTree* root)
{
    std::string data;
    std::string type = "tree";
    for(auto child: root->getChild())
    {
        std::string fileName = child->getName();
        std::string filePath = child->getFilePath();
        std::string mode = child->getMode();
        if(mode.find('0') == 0)
            mode = mode.substr(1);
        data += mode + ' ' + fileName + '\0' + hexToBinary(child->getSHA()); 
    }
    root->setSHA(writeObject(type, data, true));
}
