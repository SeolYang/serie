#pragma once
#include <filesystem>
#include <string>
#include <string_view>
#include <span>
#include <cstdint>
#include <optional>
#include <sstream>
#include <format>

namespace serie
{
    constexpr std::string_view EmbededFileExtension = ".embed";
    namespace fs = std::filesystem;

    static std::optional<std::string> BlobToEmbed(const std::span<const uint8_t> blob, const std::string_view embedName = "gEmbedBlob", const uint8_t newlinePerBytes = 8)
    {
        if (blob.data() == 0)
        {
            return std::nullopt;
        }

        constexpr std::string_view BaseEmbedingFormat = "constexpr inline uint8_t {}[] = {{ {} }};";

        std::stringstream stream;
        uint8_t counter = 0;
        for (const auto byte : blob)
        {
            if (newlinePerBytes <= counter)
            {
                stream << std::endl;
                counter = 0;
            }

            stream << std::format("{}, ", byte);
            ++counter;
        }

        return std::format(BaseEmbedingFormat, embedName, stream.str());
    }
}