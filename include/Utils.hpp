#ifndef UTILS_H
#define UTILS_H

#include <map>

std::string converTime(time_t t);

std::map<std::string, std::string> getFileStat(const std::string &filePath);

std::string decToOct(uint32_t decimal);

std::string writeObject(std::string type, std::string &data, bool write=false);

bool decompressData(std::vector<char> &compressed, std::vector<char> &decompressed);

bool compressData(std::vector<char> &decompressed, std::vector<char> &compressed);

std::string computeSHA(const std::string &input);

std::string hexToBinary(const std::string &input);

std::string computeHex(const unsigned char *input);

#endif
