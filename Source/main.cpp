#include <serie.h>
#include <CLI/CLI.hpp>

int main(int argc, char** argv)
{
    using namespace serie;
    CLI::App app{ "Embed blob to header like file" };

    std::vector<fs::path> targetPaths;
    app.add_option("-f,--filename", targetPaths, "File paths to embed")
        ->expected(1, 128)
        ->required();

    uint8_t newlineInterval = 0;
    app.add_option("-n,--newline", newlineInterval, "Newline interval")
        ->default_val(8)
        ->expected(1, 32);

    CLI11_PARSE(app, argc, argv);

    for (const fs::path& targetPath : targetPaths)
    {
        fs::path outputPath{ targetPath };
        outputPath.replace_extension(serie::EmbededFileExtension);
        std::cout << std::format("Embed blob {} to {}.\n", targetPath.string(), outputPath.string());

        if (!fs::exists(targetPath))
        {
            std::cerr << "Target path does not exist." << std::endl;
            SERIE_NOENTRY();
            continue;
        }

        const size_t sizeOfBlob = fs::file_size(targetPath);
        std::ifstream inputStream{ targetPath, std::ios::binary | std::ios::in };
        if (!inputStream.is_open())
        {
            std::cerr << "Failed to open input stream. " << targetPath << std::endl;
            SERIE_NOENTRY();
            continue;
        }

        if (sizeOfBlob == 0)
        {
            std::cerr << "The size of blob must greater than 0. " << targetPath << std::endl;
            SERIE_NOENTRY();
            continue;
        }

        std::vector<uint8_t> blob(sizeOfBlob);
        SERIE_CHECK(blob.size() == sizeOfBlob);
        inputStream.read((char*)blob.data(), blob.size());
        inputStream.close();

        const std::string embedName = targetPath.filename().replace_extension().string();
        std::optional<std::string> embededBlob = EmbedBlob(blob, embedName, newlineInterval);
        if (!embededBlob)
        {
            std::cerr << "Failed to embed blob." << targetPath << std::endl;
            continue;
        }

        std::ofstream outputStream{ outputPath };
        if (!outputStream.is_open())
        {
            std::cerr << "Failed to open output stream." << outputPath << std::endl;
            SERIE_NOENTRY();
            continue;
        }
        outputStream << *embededBlob;
        outputStream.close();
    }

    return 0;
}