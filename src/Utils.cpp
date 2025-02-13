#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <zlib.h>
#include <openssl/sha.h>

//bool decompressData(std::vector<char>& compressedData, std::vector<char> &decmp) {
//    // Initialize zlib structures
//    z_stream stream{};
//    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
//    stream.avail_in = compressedData.size();
//
//    // Allocate a buffer for decompression
//    constexpr size_t bufferSize = 4096; // 4 KB buffer
//    std::vector<char> buffer(bufferSize);
//    std::string decompressedData;
//
//    // Initialize zlib for inflation
//    if (inflateInit(&stream) != Z_OK) {
//        throw std::runtime_error("Failed to initialize zlib");
//    }
//
//    try {
//        // Decompress in chunks
//        int ret;
//        do {
//            stream.next_out = reinterpret_cast<Bytef*>(buffer.data());
//            stream.avail_out = buffer.size();
//            ret = inflate(&stream, Z_NO_FLUSH);
//            if (ret != Z_OK && ret != Z_STREAM_END) {
//                inflateEnd(&stream);
//                throw std::runtime_error("Zlib decompression error");
//            }
//
//            decompressedData.append(buffer.data(), buffer.size() - stream.avail_out);
//        } while (ret != Z_STREAM_END);
//
//        // Clean up
//        inflateEnd(&stream);
//    } catch (...) {
//        inflateEnd(&stream);
//        throw;
//    }
//    int j=0;
//    uLong siz = decompressedData.size();
//    decmp.resize(siz);
//    for(auto i: decompressedData)
//    {
//        decmp[j++] = i;
//    }return true;
//}

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

std::string computeHex(const unsigned char *input)
{
    std::ostringstream ss;
    for (int i=0; i<20; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(input[i]));
    }
    return ss.str();
}
