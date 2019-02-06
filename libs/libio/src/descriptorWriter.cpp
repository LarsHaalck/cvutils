#include "io/descriptorWriter.h"

#include <iostream>

#include <opencv2/features2d.hpp>

#include "io/config.h"

namespace cvutils
{
DescriptorWriter::DescriptorWriter(const std::filesystem::path& ftDir)
    : mFtDir(ftDir)
{
    std::filesystem::create_directory(ftDir);
}


bool DescriptorWriter::writeDescriptors(const std::string& imgName,
    const cv::Mat& descriptors)
{
    std::filesystem::path imgStem(imgName);
    imgStem = mFtDir / imgStem.stem();
    cv::FileStorage fsDesc(imgStem.string() + detail::descEnding, cv::FileStorage::WRITE);
    if (!fsDesc.isOpened())
    {
        std::cout << "Could not open descriptor file for writing" << std::endl;
        return false;
    }
    else
        cv::write(fsDesc, detail::descKey, descriptors);

    return true;
}

} // namespace cvutils
