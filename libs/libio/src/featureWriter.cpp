#include "io/featureWriter.h"

#include <fstream>

#include "io/config.h"

namespace cvutils
{
FeatureWriter::FeatureWriter(const std::filesystem::path& ftDir)
    : mFtDir(ftDir)
{
    std::filesystem::create_directory(ftDir);
}

void FeatureWriter::writeFeatures(const std::string& imgName,
        const std::vector<cv::KeyPoint>& features)
{
    std::filesystem::path imgStem(imgName);
    imgStem = mFtDir / imgStem.stem();
    std::filesystem::path fileName(imgStem.string() + detail::ftEnding);

    std::ofstream stream (fileName.string(), std::ios::out | std::ios::binary);
    if (!stream.is_open())
    {
        throw std::filesystem::filesystem_error("Error opening feature file",
                fileName, std::make_error_code(std::errc::io_error));
    }

    {
        cereal::PortableBinaryOutputArchive archive(stream);
        archive(features);
        stream.close();
    }
}
} // namespace cvutils
