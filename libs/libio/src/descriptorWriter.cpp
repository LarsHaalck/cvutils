#include "io/descriptorWriter.h"

#include <opencv2/features2d.hpp>

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
    cv::FileStorage fsDesc(fileName, cv::FileStorage::WRITE);
    if (!fsDesc.isOpened())
    {
        throw std::filesystem::filesystem_error("Error opening descriptor file",
            fileName, std::make_error_code(std::errc::io_error));
    }

    cv::write(fsDesc, detail::descKey, descriptors);

}

} // namespace cvutils
