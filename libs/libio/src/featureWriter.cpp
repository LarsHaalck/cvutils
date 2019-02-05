#include "io/featureWriter.h"

#include <iostream>
#include <opencv2/features2d.hpp>

namespace cvutils
{
namespace io
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
    cv::FileStorage fsFt(imgStem.string() + "-feat.yml.gz", cv::FileStorage::WRITE);

    if (!fsFt.isOpened())
    {
        std::cout << "Could not open feature file for writing" << std::endl;
        return false;
    }
    else
        cv::write(fsFt, "pts", features);

    return true;
}

bool FeatureWriter::writeDescriptors(const std::string& imgName,
    const cv::Mat& descriptors)
{
    std::filesystem::path imgStem(imgName);
    imgStem = mFtDir / imgStem.stem();
    cv::FileStorage fsDesc(imgStem.string() + "-desc.yml.gz", cv::FileStorage::WRITE);
    if (!fsDesc.isOpened())
    {
        std::cout << "Could not open descriptor file for writing" << std::endl;
        return false;
    }
    else
        cv::write(fsDesc, "desc", descriptors);

    return true;
}

bool FeatureWriter::writeFeaturesAndDescriptors(const std::string& imgName,
    const std::vector<cv::KeyPoint>& features, const cv::Mat& descriptors)
{
    return (writeFeatures(imgName, features)
        && writeDescriptors(imgName, descriptors));
}

} // namespace io
} // namespace cvutils
