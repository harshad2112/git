#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <filesystem>

#include "GitUtils.hpp"
#include "IndexEntry.hpp"
#include "Utils.hpp"

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

bool sortByFileName(IndexEntry &a, IndexEntry &b)
{
    return a.path < b.path;
}

void appendBigEndian(std::string &buffer, uint32_t value)
{
    uint32_t bigEndianVal = htonl(value);
    buffer.append(reinterpret_cast<const char*> (&bigEndianVal), sizeof(bigEndianVal));
}

void writeGitIndex(std::vector<IndexEntry> currentFiles, std::string folderPath)
{
    sort(currentFiles.begin(), currentFiles.end(), sortByFileName);
    std::filesystem::path indexPath = folderPath + "/.git/index";
    std::ofstream indexFile(indexPath, std::ios::binary);
    if(!indexFile.is_open())
    {
        throw std::runtime_error("fatal: unable to open index file");
    }
    std::string indexData;
    std::string signature = "DIRC";
    uint32_t version = 3, entryCount = currentFiles.size();

    indexData.append("DIRC", 4);
    appendBigEndian(indexData, version);
    appendBigEndian(indexData, entryCount);
    for(auto entry: currentFiles)
    {
        appendBigEndian(indexData, entry.ctimeSec);
        appendBigEndian(indexData, entry.ctimeMSec);
        appendBigEndian(indexData, entry.mtimeSec);
        appendBigEndian(indexData, entry.mtimeMSec);
        appendBigEndian(indexData, entry.dev);
        appendBigEndian(indexData, entry.ino);
        appendBigEndian(indexData, entry.mode);
        appendBigEndian(indexData, entry.uid);
        appendBigEndian(indexData, entry.gid);
        appendBigEndian(indexData, entry.size);
        std::string binSha = hexToBinary(entry.sha1);
        indexData.append(reinterpret_cast<const char*> (binSha.c_str()), 20);
        uint16_t flagEndian = htons(entry.flag);
        indexData.append(reinterpret_cast<const char*> (&flagEndian), sizeof(flagEndian));
        indexData.append(reinterpret_cast<const char*> (entry.path.c_str()), entry.path.size());
        indexData += '\0';

        int padding = (8 - ((62 + entry.path.size() + 1) %8 )) %8;
        indexData += std::string(padding, 0);
    }
    indexFile << indexData;
    indexFile.close();
}

std::vector<std::string> getGitIgnoreFiles(std::string folderPath)
{
    std::filesystem::path gitIgnoreFilePath = folderPath + "/.gitignore";
    std::ifstream file(gitIgnoreFilePath, std::ios::binary);
    if(!file.is_open())
    {
        throw std::runtime_error("fatal: unable to open index file");
    }
    std::vector<std::string> gitIgnoreFiles;
    std::string filename;
    while(getline(file, filename))
    {
        gitIgnoreFiles.push_back(folderPath + "/" + filename);
    }
    return gitIgnoreFiles;
}

std::vector<IndexEntry> getAllFiles(std::string folderPath)
{
    std::vector<IndexEntry> currentFiles;
    std::vector<std::string> gitIgnoreFiles = getGitIgnoreFiles(folderPath);
    for(const auto &dirEntry: recursive_directory_iterator(folderPath))
    {
        bool skip = false;
        for(auto gitIgnorePath: gitIgnoreFiles)
        {
            if(dirEntry.path().string().find(gitIgnorePath)==0)
            {
                skip = true;
            }
        }
        if(!skip)
        {
            std::map<std::string, std::string> fileData = getFileStat(dirEntry.path().string());
            if(fileData["mode"] == "16877") continue;
            struct IndexEntry currentEntry(fileData);
            currentEntry.path.erase(currentEntry.path.begin(), currentEntry.path.begin() + folderPath.size() + 1);
            currentEntry.flag = currentEntry.path.size();
            currentFiles.push_back(currentEntry);
        }
    }
    return currentFiles;
}

std::string writeObject(std::string type, std::string &data, bool write)
{
    std::string dataToCompress = type + " " + std::to_string(data.size()) + '\0' + data;
    std::string shaString = computeSHA(dataToCompress);
    if (write)
    {
        std::vector<char> compressed;
        std::vector<char> input(dataToCompress.begin(), dataToCompress.end());
        if (compressData(input, compressed))
        {
            std::string data(compressed.begin(), compressed.end());
            std::string folder = shaString.substr(0, 2);
            std::string file = shaString.substr(2);
            std::filesystem::path currentFolder = std::filesystem::current_path();
            std::filesystem::path folderPath = currentFolder / ".git/objects/" / folder;
            if (!std::filesystem::exists(folderPath))
            {
                std::filesystem::create_directories(folderPath);
            }
            std::filesystem::path filePath = currentFolder / ".git/objects/" / folder / file;
            auto commitFile = std::ofstream(filePath);
            if (!commitFile.is_open())
            {
                if(errno == EACCES)
                    return shaString;
                throw std::runtime_error("fatal: Not able to write to file " + folder + file);
            }
            commitFile << data;
            commitFile.close();
        }
        else
        {
            throw std::runtime_error("fatal: Not a valid object name");
        }
    }
    return shaString;
}

std::vector<IndexEntry> readGitIndex(const std::string &indexPath)
{
    std::ifstream indexFile(indexPath, std::ios::binary);
    if(!indexFile.is_open())
    {
        throw std::runtime_error("fatal: unable to open index file");
    }
    char signature[4];
    indexFile.read(signature, 4);
    if(std::strncmp(signature, "DIRC", 4) != 0)
    {
        throw std::runtime_error("fatal: Invalid git index file");
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
        entry.flag = ntohs(entry.flag);

        getline(indexFile, entry.path, '\0');

        int padding = (8 - ((62 + entry.path.size() + 1) %8 )) %8;
        indexFile.seekg(padding, std::ios::cur);

        indexEntries.push_back(entry);
    }
    return indexEntries;
}

void printTreeOutput(std::string &flag, std::string &mode, std::string &type, std::string &data, std::string &siz, std::string &name)
{
    if(flag == "" or flag=="-r")
    {
        std::cout<<mode<<" "<<type<<" "<<data<<"    "<<name<<"\n";
    }
    else if(flag == "-l")
    {
        siz = std::string(58 - (siz.size() + mode.size() + type.size() + data.size()), ' ') + siz;
        std::cout<<mode<<" "<<type<<" "<<data<<" "<<siz<<"    "<<name<<"\n";
    }
}

std::string getFileData(std::string value)
{
    std::string folder = value.substr(0, 2);
    std::string file = value.substr(2);
    std::filesystem::path currentFolder = std::filesystem::current_path();
    std::filesystem::path filePath = currentFolder / ".git/objects/" / folder / file;
    auto commitFile = std::ifstream(filePath, std::ios::binary);
    if (!commitFile.is_open())
    {
        throw std::runtime_error("fatal: Not a valid object name " + folder + file);
        return "";
    }
    std::vector<char> compressed((std::istreambuf_iterator<char>(commitFile)), {});
    commitFile.close();

    std::vector<char> decompressed;
    if (decompressData(compressed, decompressed))
    {
        std::string data(decompressed.begin(), decompressed.end());
        return data;
    }
    else
    {
        throw std::runtime_error("fatal: Not a valid object name " + folder + file);
    }
}

void extractHash(std::string hash, std::string flag)
{
    std::string fileData = getFileData(hash);
    int nullDelimiter = fileData.find('\0');
    std::string header = fileData.substr(0, nullDelimiter);
    std::string data = fileData.substr(nullDelimiter);
    if(header.find("tree")==-1)
    {
        std::cout<<"Not a tree\n";
        return;
    }
    std::istringstream iss(data);
    while(iss)
    {
        std::string mode, name;
        std::getline(iss, mode, ' ');
        std::getline(iss, name, '\0');
        if(mode.size()<6)
        {
            mode = std::string(6 - mode.size(), '0') + mode;
        }
        char sha1Binary[20];
        iss.read(sha1Binary, 20);
        if(iss)
        {
            std::string hexData = computeHex(reinterpret_cast<unsigned char *> (sha1Binary));
            std::string objectData = getFileData(hexData);
            nullDelimiter = objectData.find('\0');
            header = objectData.substr(0, nullDelimiter);
            int spaceDelimiter = header.find(' ');
            std::string type = header.substr(0, spaceDelimiter);
            if(type=="tree" and flag == "-r")
            {
                extractHash(hexData, flag);
            }
            else
            {
                std::string objectSize = header.substr(spaceDelimiter);
                printTreeOutput(flag, mode, type, hexData, objectSize, name);
            }
        }
    }

}
