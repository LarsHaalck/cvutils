#ifndef CVUTILS_DESCRIPTOR_FETCHER_H
#define CVUTILS_DESCRIPTOR_FETCHER_H

#include <filesystem>
#include <vector>

#include <opencv2/core.hpp>

#include "fetch/abstractFetcher.h"
#include "fetch/imageFetcher.h"
#include "io/config.h"

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

        cv::FileStorage fsFt(fileName.string(),
            cv::FileStorage::READ);

        if (!fsFt.isOpened())
        {
            throw std::filesystem::filesystem_error("Error opening matches file",
                fileName, std::make_error_code(std::errc::io_error));
        }

        cv::Mat currDesc;
        cv::read(fsFt[detail::descKey], currDesc);

        return currDesc;
    }

};
} // namespace detail
} // namespace cvutils

#endif // CVUTILS_DESCRIPTOR_FETCHER_H
