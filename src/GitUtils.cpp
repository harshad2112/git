#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>


#include "../include/GitUtils.hpp"
#include "../include/IndexEntry.hpp"
#include "../include/Utils.hpp"

std::string cleanPath(std::string path)
{
    size_t start = path.find_first_not_of(" \n\r\t");
    if(start == std::string::npos)
        return "";
    size_t end = path.find_last_not_of("     \n\r\t");
    return path.substr(start, end - start + 1);
}

std::vector<IndexEntry> readGitIndex(const std::string &indexPath)
{
    std::ifstream indexFile(indexPath, std::ios::binary);
    if(!indexFile.is_open())
    {
        throw("fatal: unable to open index file");
    }        
    char signature[4];
    indexFile.read(signature, 4);
    if(std::strncmp(signature, "DIRC", 4) != 0)
    {
        throw("fatal: Invalid git index file");
    }
    
    uint32_t version, entryCount;
    indexFile.read(reinterpret_cast<char*>(&version), 4);
    version = ntohl(version);

    indexFile.read(reinterpret_cast<char*>(&entryCount), 4);
    entryCount = ntohl(entryCount);
    
    std::vector<IndexEntry> indexEntries;
    for(uint32_t i=0; i<entryCount; i++)
    {
        IndexEntry entry;
        indexFile.read(reinterpret_cast<char*>(&entry.ctimeSec), 4); 
        entry.ctimeSec = ntohl(entry.ctimeSec);
        indexFile.read(reinterpret_cast<char*>(&entry.ctimeMSec), 4); 
        entry.ctimeMSec = ntohl(entry.ctimeMSec);
        indexFile.read(reinterpret_cast<char*>(&entry.mtimeSec), 4); 
        entry.mtimeSec = ntohl(entry.mtimeSec);
        indexFile.read(reinterpret_cast<char*>(&entry.mtimeMSec), 4); 
        entry.mtimeMSec = ntohl(entry.mtimeMSec);
        indexFile.read(reinterpret_cast<char*>(&entry.dev), 4); 
        entry.dev = ntohl(entry.dev);
        indexFile.read(reinterpret_cast<char*>(&entry.ino), 4); 
        entry.ino = ntohl(entry.ino);
        indexFile.read(reinterpret_cast<char*>(&entry.mode), 4); 
        entry.mode = ntohl(entry.mode);
        indexFile.read(reinterpret_cast<char*>(&entry.uid), 4); 
        entry.uid = ntohl(entry.uid);
        indexFile.read(reinterpret_cast<char*>(&entry.gid), 4); 
        entry.gid = ntohl(entry.gid);
        indexFile.read(reinterpret_cast<char*>(&entry.size), 4); 
        entry.size = ntohl(entry.size);
        
        unsigned char sha1[20];
        indexFile.read(reinterpret_cast<char*>(sha1), 20);
        entry.sha1 = computeHex(sha1);
        indexFile.read(reinterpret_cast<char*>(&entry.flag), 2); 
        entry.flag = ntohl(entry.flag);
        
        getline(indexFile, entry.path, '\0');
        
        int padding = (8 - ((62 + entry.path.size() + 1) %8 )) %8;
        indexFile.seekg(padding, std::ios::cur);
        
        indexEntries.push_back(entry);
    }
    return indexEntries;
} 
