#include <map>
#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
#include <zlib.h>
#include <ctime>
#include <sys/stat.h>
#include <openssl/sha.h>

std::string convertTime(time_t t)
{
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buffer);
}

std::map<std::string, std::string> getFileStat(const std::string &filePath)
{
    struct stat fileStat;
    std::map<std::string, std::string> metaData;
    if(stat(filePath.c_str(), &fileStat) == 0)
    {
        metaData["path"] = filePath;
        metaData["size"] = std::to_string(fileStat.st_size);
        metaData["mode"] = std::to_string(fileStat.st_mode & 0777);
        metaData["uid"] = std::to_string(fileStat.st_uid);
        metaData["gid"] = std::to_string(fileStat.st_gid);
        metaData["dev"] = std::to_string(fileStat.st_dev);
        metaData["ino"] = std::to_string(fileStat.st_ino);
        metaData["mtime"] = convertTime(fileStat.st_mtime);
        metaData["ctime"] = convertTime(fileStat.st_ctime);
    }
    return metaData;
}

std::string decToOct(uint32_t decimal)
{
    std::stringstream st;
    st << std::oct << decimal;
    return st.str();
}

// This function is generated using chatGPT.

/*
bool decompressData(std::vector<char>& compressedData, std::vector<char> &decmp) {
    // Initialize zlib structures
    z_stream stream{};
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = compressedData.size();

    // Allocate a buffer for decompression
    constexpr size_t bufferSize = 4096; // 4 KB buffer
    std::vector<char> buffer(bufferSize);
    std::string decompressedData;

    // Initialize zlib for inflation
    if (inflateInit(&stream) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib");
    }

    try {
        // Decompress in chunks
        int ret;
        do {
            stream.next_out = reinterpret_cast<Bytef*>(buffer.data());
            stream.avail_out = buffer.size();
            ret = inflate(&stream, Z_NO_FLUSH);
            if (ret != Z_OK && ret != Z_STREAM_END) {
                inflateEnd(&stream);
                throw std::runtime_error("Zlib decompression error");
            }

            decompressedData.append(buffer.data(), buffer.size() - stream.avail_out);
        } while (ret != Z_STREAM_END);

        // Clean up
        inflateEnd(&stream);
    } catch (...) {
        inflateEnd(&stream);
        throw;
    }
    int j=0;
    uLong siz = decompressedData.size();
    decmp.resize(siz);
    for(auto i: decompressedData)
    {
        decmp[j++] = i;
    }return true;
}
*/

// Below function is working fine for now, if there seems to be any error
// we can use the above function

bool decompressData(std::vector<char> &compressed, std::vector<char> &decompressed)
{
    uLong decompressedSize = compressed.size() * 4;
    decompressed.resize(decompressedSize);

    int result;
    while ((result = uncompress(reinterpret_cast<Bytef *>(decompressed.data()), &decompressedSize, reinterpret_cast<const Bytef *>(compressed.data()), compressed.size())) == Z_BUF_ERROR)
    {
        decompressedSize*=2;
        decompressed.resize(decompressedSize);
    }

    if (result != Z_OK)
    {
        std::cerr << "fatal: not a valid file\n";
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

std::string hexToBinary(const std::string &input)
{
    std::string st;
    for(size_t i=0; i<input.length(); i+=2)
    {   
        unsigned int byte;
        std::istringstream(input.substr(i, 2)) >> std::hex >> byte;
        st += static_cast<unsigned char>(byte);
    }
    return st;
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
