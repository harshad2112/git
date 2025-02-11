#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
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

bool initializeGit(int argc, char *argv[])
{
    try
    {
        std::filesystem::create_directory(".git");
        std::filesystem::create_directory(".git/objects");
        std::filesystem::create_directory(".git/refs");

        std::ofstream headFile(".git/HEAD");
        if (headFile.is_open())
        {
            headFile << "ref: refs/heads/main\n";
            headFile.close();
        }
        else
        {
            std::cerr << "Failed to create .git/HEAD file.\n";
            return EXIT_FAILURE;
        }

        std::cout << "Initialized git directory\n";
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}

bool handleCatFile(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Invalid arguments, required a flag and blob-sha.\n";
        return EXIT_FAILURE;
    }
    std::string flag = argv[2];
    std::string value = argv[3];
    if (flag == "-p")
    {
        std::string folder = value.substr(0, 2);
        std::string file = value.substr(2);
        std::filesystem::path currentFolder = std::filesystem::current_path();
        std::filesystem::path filePath = currentFolder / ".git/objects/" / folder / file;
        auto commitFile = std::ifstream(filePath);
        if (!commitFile.is_open())
        {
            std::cerr << "fatal: Not a valid object name " << folder << file << "\n";
            return EXIT_FAILURE;
        }
        std::vector<char> compressed((std::istreambuf_iterator<char>(commitFile)), {});
        commitFile.close();

        std::vector<char> decompressed;
        if (decompressData(compressed, decompressed))
        {
            std::string data(decompressed.begin(), decompressed.end());
            int delimiterPos = data.find('\0');
            std::cout << data.substr(delimiterPos + 1);
        }
        else
        {
            std::cerr << "fatal: Not a valid object name " << folder << file << "\n";
            return EXIT_FAILURE;
        }
    }
    return 0;
}

bool handleHashing(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Invalid arguments, required a flag and blob-sha.\n";
        return EXIT_FAILURE;
    }
    std::string flag = argv[2];
    std::string file;
    if (argc == 4)
    {
        file = argv[3];
    }
    else
    {
        file = flag;
        flag = "";
    }
    std::filesystem::path currentFolder = std::filesystem::current_path();
    std::filesystem::path filePath = currentFolder / file;
    auto commitFile = std::ifstream(filePath);
    if (!commitFile.is_open())
    {
        std::cerr << "fatal: Not a valid object name " << file << "\n";
        return EXIT_FAILURE;
    }
    std::vector<char> decompressed((std::istreambuf_iterator<char>(commitFile)), {});
    commitFile.close();

    std::string fileData(decompressed.begin(), decompressed.end());
    std::string dataToCompress = "blob " + std::to_string(fileData.size()) + '\0' + fileData;
    std::string shaString = computeSHA(dataToCompress);
    std::cout << shaString << "\n";

    if (flag == "-w")
    {
        std::vector<char> compressed;
        std::vector<char> input(dataToCompress.begin(), dataToCompress.end());
        if (compressData(input, compressed))
        {
            std::string data(compressed.begin(), compressed.end());
            std::string folder = shaString.substr(0, 2);
            std::string file = shaString.substr(2);
            std::filesystem::path folderPath = currentFolder / ".git/objects/" / folder;
            if (!std::filesystem::exists(folderPath))
            {
                std::filesystem::create_directories(folderPath);
            }
            filePath = currentFolder / ".git/objects/" / folder / file;
            auto commitFile = std::ofstream(filePath);
            if (!commitFile.is_open())
            {
                std::cerr << "fatal: Not able to write to file " << folder << file << "\n";
                return EXIT_FAILURE;
            }
            commitFile << data;
            commitFile.close();
        }
        else
        {
            std::cerr << "fatal: Not a valid object name " << file << "\n";
            return EXIT_FAILURE;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2)
    {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }

    std::string command = argv[1];

    if (command == "init")
    {
        initializeGit(argc, argv);
    }
    else if (command == "cat-file")
    {
        handleCatFile(argc, argv);
    }
    else if (command == "hash-object")
    {
        handleHashing(argc, argv);
    }
    else
    {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

