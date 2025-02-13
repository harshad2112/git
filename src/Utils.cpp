#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <zlib.h>
#include <openssl/sha.h>

bool decompressData(std::vector<char> &compressed, std::vector<char> &decompressed)
{
    uLong decompressedSize = compressed.size() * 4;
    decompressed.resize(decompressedSize);

    int result = uncompress(reinterpret_cast<Bytef *>(decompressed.data()), &decompressedSize, reinterpret_cast<const Bytef *>(compressed.data()), compressed.size());

    if (result != Z_OK)
    {
        std::cerr << "Fatal: not a valid file\n";
        return false;
    }
    decompressed.resize(decompressedSize);
    return true;
}

bool compressData(std::vector<char> &decompressed, std::vector<char> &compressed)
{
    uLong compressedSize = compressBound(decompressed.size());
    compressed.resize(compressedSize);

    int result = compress(reinterpret_cast<Bytef *>(compressed.data()), &compressedSize, reinterpret_cast<const Bytef *>(decompressed.data()), decompressed.size());

    if (result != Z_OK)
    {
        std::cerr << "Fatal: not a valid file\n";
        return false;
    }
    compressed.resize(compressedSize);
    return true;
}

std::string computeSHA(const std::string &input)
{
    unsigned char hash[20];
    SHA1((unsigned char *)input.c_str(), input.size(), hash);

    std::stringstream ss;
    for (const auto &byte : hash)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
}

std::string computeHex(const unsigned char *input)
{
    std::ostringstream ss;
    for (int i=0; i<20; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(input[i]));
    }
    return ss.str();
}
