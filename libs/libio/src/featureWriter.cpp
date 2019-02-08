#include "io/featureWriter.h"

#include <iostream>
#include <opencv2/features2d.hpp>

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
    cv::FileStorage fsFt(fileName, cv::FileStorage::WRITE);

    if (!fsFt.isOpened())
    {
        throw std::filesystem::filesystem_error("Error opening feature file",
            fileName, std::make_error_code(std::errc::io_error));
    }
    cv::write(fsFt, detail::ftKey, features);
}
} // namespace cvutils
