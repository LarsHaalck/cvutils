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

bool FeatureWriter::writeFeatures(const std::string& imgName,
    const std::vector<cv::KeyPoint>& features)
{
    std::filesystem::path imgStem(imgName);
    imgStem = mFtDir / imgStem.stem();
    cv::FileStorage fsFt(imgStem.string() + detail::ftEnding, cv::FileStorage::WRITE);

    if (!fsFt.isOpened())
    {
        std::cout << "Could not open feature file for writing" << std::endl;
        return false;
    }
    else
        cv::write(fsFt, detail::ftKey, features);

    return true;
}
} // namespace cvutils
