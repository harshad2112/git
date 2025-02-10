#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <zlib.h>

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
    }
    else if (command == "cat-file")
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
    }
    else
    {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
