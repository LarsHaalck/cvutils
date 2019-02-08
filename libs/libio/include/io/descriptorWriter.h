#ifndef CVUTILS_DESCRIPTOR_WRITER_H
#define CVUTILS_DESCRIPTOR_WRITER_H

#include <filesystem>
#include <string>

namespace cv
{
    class Mat;
}

namespace cvutils
{
class DescriptorWriter
{
private:
    std::filesystem::path mFtDir;
public:
    DescriptorWriter(const std::filesystem::path& ftDir);
    void writeDescriptors(const std::string& imgFilePath,
        const cv::Mat& descriptors);

};
} // namespace cvutils

#endif // CVUTILS_DESCRIPTOR_WRITER_H
