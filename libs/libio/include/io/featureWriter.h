#ifndef CVUTILS_IO_FEATURE_WRITE_H
#define CVUTILS_IO_FEATURE_WRITE_H

#include <filesystem>
#include <string>
#include <vector>

namespace cv
{
    class KeyPoint;
}

namespace cvutils
{
class FeatureWriter
{
private:
    std::filesystem::path mFtDir;
public:
    FeatureWriter(const std::filesystem::path& ftDir);
    bool writeFeatures(const std::string& imgFilePath,
        const std::vector<cv::KeyPoint>& features);
};
} // namespace cvutils

#endif // CVUTILS_IO_FEATURE_WRITE_H
