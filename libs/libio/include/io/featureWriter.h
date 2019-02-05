#ifndef CVUTILS_IO_FEATURE_WRITE_H
#define CVUTILS_IO_FEATURE_WRITE_H

#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/core.hpp>

namespace cv
{
    class Mat;
    class Keypoint;
}

namespace cvutils
{
namespace io
{
class FeatureWriter
{
private:
    std::filesystem::path mFtDir;
public:
    FeatureWriter(const std::filesystem::path& ftDir);
    bool writeFeatures(const std::string& imgFilePath,
        const std::vector<cv::KeyPoint>& features);
    bool writeDescriptors(const std::string& imgFilePath,
        const cv::Mat& descriptors);
    bool writeFeaturesAndDescriptors(const std::string& imgFilePath,
        const std::vector<cv::KeyPoint>& features, const cv::Mat& descriptors);


};
} // namespace io
} // namespace cvutils

#endif // CVUTILS_IO_FEATURE_WRITE_H
