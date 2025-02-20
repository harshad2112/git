#ifndef INDEX_ENTRY_H
#define INDEX_ENTRY_H

#include <map>
#include <string>

struct IndexEntry 
{
    uint32_t ctimeSec;
    uint32_t ctimeMSec;
    uint32_t mtimeSec;
    uint32_t mtimeMSec;
    uint32_t dev;
    uint32_t ino;
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t size;
    uint16_t flag;
    std::string sha1;
    std::string path;
    
    IndexEntry()
    {};
    
    IndexEntry(const std::map<std::string, std::string> &fileStats)
        : ctimeSec(std::stoi(fileStats.at("ctime"))),
         mtimeSec(std::stoi(fileStats.at("mtime"))),
         dev(std::stoi(fileStats.at("dev"))),
         ino(std::stoi(fileStats.at("ino"))),
         mode(std::stoi(fileStats.at("mode"))),
         uid(std::stoi(fileStats.at("uid"))),
         gid(std::stoi(fileStats.at("gid"))),
         size(std::stoi(fileStats.at("size"))),
         path(fileStats.at("path")) {}
};

#endif
