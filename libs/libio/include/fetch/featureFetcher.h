#ifndef CVUTILS_FEATURE_FETCHER_H
#define CVUTILS_FEATURE_FETCHER_H

#define CEREAL_THREAD_SAFE 1
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>

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
void load(Archive & archive, cv::KeyPoint& kp)
{
    archive(kp.pt.x, kp.pt.y, kp.size, kp.angle, kp.response, kp.octave, kp.class_id);
}
}

namespace cvutils
{
namespace detail
{
class FeatureFetcher : public AbstractFetcher<size_t, std::vector<cv::KeyPoint>>
{
private:
    ImageFetcher mImgFetcher;
    std::filesystem::path mFtDir;
public:
    FeatureFetcher(const std::filesystem::path& imgDir,
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

    std::vector<cv::KeyPoint> get(const size_t& idx) const override
    {
        std::filesystem::path imgStem(mImgFetcher.getImagePath(idx));
        imgStem = mFtDir / imgStem.stem();
        std::filesystem::path fileName(imgStem.string() + detail::ftEnding);

        std::ifstream stream (fileName.string(), std::ios::in | std::ios::binary);
        if (!stream.is_open())
        {
            throw std::filesystem::filesystem_error("Error opening feature file",
                    fileName, std::make_error_code(std::errc::io_error));
        }

        std::vector<cv::KeyPoint> features;
        {
            cereal::PortableBinaryInputArchive archive(stream);
            archive(features);
            stream.close();
        }

        return features;
    }

};
} // namespace detail
} // namespace cvutils

#endif // CVUTILS_FEATURE_FETCHER_H
