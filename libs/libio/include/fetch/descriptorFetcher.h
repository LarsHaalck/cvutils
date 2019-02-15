#ifndef CVUTILS_DESCRIPTOR_FETCHER_H
#define CVUTILS_DESCRIPTOR_FETCHER_H

#define CEREAL_THREAD_SAFE 1
#include <cereal/archives/portable_binary.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

#include <opencv2/core.hpp>

#include "fetch/abstractFetcher.h"
#include "fetch/imageFetcher.h"
#include "io/config.h"

namespace cereal
{
template<class Archive>
void load(Archive& archive, cv::Mat& mat)
{
    int rows, cols, type;
    bool continuous;
    archive(rows, cols, type, continuous);

    if (continuous)
    {
        mat.create(rows, cols, type);
        int dataSize = rows * cols * static_cast<int>(mat.elemSize());
        archive(cereal::binary_data(mat.ptr(), dataSize));
    }
    else
    {
        mat.create(rows, cols, type);
        int rowSize = cols * static_cast<int>(mat.elemSize());
        for (int i = 0; i < rows; i++)
            archive(cereal::binary_data(mat.ptr(i), rowSize));
    }
}
}

namespace cvutils
{
namespace detail
{
class DescriptorFetcher : public AbstractFetcher<size_t, cv::Mat>
{
private:
    ImageFetcher mImgFetcher;
    std::filesystem::path mFtDir;
public:
    DescriptorFetcher(const std::filesystem::path& imgDir,
        const std::filesystem::path& txtFile, const std::filesystem::path& ftDir)
    : mImgFetcher(imgDir, txtFile)
    , mFtDir(ftDir)
    {
        if (!std::filesystem::exists(mFtDir)
            || !std::filesystem::is_directory(mFtDir))
        {
            throw std::filesystem::filesystem_error("Feature folder does not exist",
                mFtDir, std::make_error_code(std::errc::no_such_file_or_directory));
        }
    }

    size_t size() const override { return mImgFetcher.size(); }

    cv::Mat get(const size_t& idx) const override
    {
        std::filesystem::path imgStem(mImgFetcher.getImagePath(idx));
        imgStem = mFtDir / imgStem.stem();
        std::filesystem::path fileName(imgStem.string() + detail::descEnding);

        std::ifstream stream (fileName.string(), std::ios::in | std::ios::binary);
        if (!stream.is_open())
        {
            throw std::filesystem::filesystem_error("Error opening descriptor file",
                    fileName, std::make_error_code(std::errc::io_error));
        }

        cv::Mat descriptors;
        {
            cereal::PortableBinaryInputArchive archive(stream);
            archive(descriptors);
            stream.close();
        }

        return descriptors;
    }

};
} // namespace detail
} // namespace cvutils

#endif // CVUTILS_DESCRIPTOR_FETCHER_H
