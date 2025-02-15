#ifndef INDEX_ENTRY_H
#define INDEX_ENTRY_H

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
};

#endif
