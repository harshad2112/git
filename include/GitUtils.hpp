#ifndef GIT_UTIL_H
#define GIT_UTIL_H

#include <vector>

#include "IndexEntry.hpp"

void writeGitIndex(std::vector<IndexEntry> currentFiles, std::string folderPath);

std::vector<IndexEntry> getAllFiles(std::string folderPath);

std::string writeObject(std::string type, std::string &data, bool write);

std::vector<IndexEntry> readGitIndex(const std::string &indexPath);

std::string getFileData(std::string value);

void extractHash(std::string hash, std::string flag);

void printTreeOutput(std::string &flag, std::string &mode, std::string &type, std::string &data, std::string &siz, std::string &name);

#endif
