#ifndef CVUTILS_DESCRIPTOR_WRITER_H
#define CVUTILS_DESCRIPTOR_WRITER_H

#define CEREAL_THREAD_SAFE 1
#include <cereal/archives/portable_binary.hpp>

#include <filesystem>
#include <string>

#include <opencv2/core.hpp>

namespace cereal
{
template<class Archive>
void save(Archive& archive, const cv::Mat& mat)
{
    int rows = mat.rows;
    int cols = mat.cols;
    int type = mat.type();
    bool continuous = mat.isContinuous();

    archive(rows, cols, type, continuous);

    if (continuous)
    {
        // store as one continues array
        int dataSize = rows * cols * static_cast<int>(mat.elemSize());
        archive(cereal::binary_data(mat.ptr(), dataSize));
    }
    else
    {
        // store row-wise
        int rowSize = cols * static_cast<int>(mat.elemSize());
        for (int i = 0; i < rows; i++)
            archive(cereal::binary_data(mat.ptr(i), rowSize));
    }
}
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
