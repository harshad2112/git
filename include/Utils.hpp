#ifndef UTILS_H
#define UTILS_H

bool decompressData(std::vector<char> &compressed, std::vector<char> &decompressed);

bool compressData(std::vector<char> &decompressed, std::vector<char> &compressed);

std::string computeSHA(const std::string &input);

std::string computeHex(const unsigned char *input);

#endif
