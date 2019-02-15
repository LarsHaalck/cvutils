#include "io/descriptorWriter.h"

#include <fstream>

#include "io/config.h"

namespace cvutils
{
DescriptorWriter::DescriptorWriter(const std::filesystem::path& ftDir)
    : mFtDir(ftDir)
{
    std::filesystem::create_directory(ftDir);
}


void DescriptorWriter::writeDescriptors(const std::string& imgName,
    const cv::Mat& descriptors)
{
    std::filesystem::path imgStem(imgName);
    imgStem = mFtDir / imgStem.stem();
    std::filesystem::path fileName(imgStem.string() + detail::descEnding);


    std::ofstream stream (fileName.string(), std::ios::out | std::ios::binary);
    if (!stream.is_open())
    {
        throw std::filesystem::filesystem_error("Error opening descriptor file",
                fileName, std::make_error_code(std::errc::io_error));
    }

    {
        cereal::PortableBinaryOutputArchive archive(stream);
        archive(descriptors);
        stream.close();
    }
}

} // namespace cvutils
