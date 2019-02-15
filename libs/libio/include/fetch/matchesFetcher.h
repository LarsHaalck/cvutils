#ifndef CVUTILS_MATCHES_FETCHER_H
#define CVUTILS_MATCHES_FETCHER_H

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
class MatchesFetcher : public AbstractFetcher<std::pair<size_t, size_t>,
    std::vector<cv::DMatch>>
{
private:
    cv::FileStorage mFile;
    cv::Mat mPairMat;
public:
    MatchesFetcher(const std::filesystem::path& ftDir, MatchType type)
    {
        if (!std::filesystem::exists(ftDir)
            || !std::filesystem::is_directory(ftDir))
        {
            throw std::filesystem::filesystem_error("Feature folder does not exist",
                ftDir, std::make_error_code(std::errc::no_such_file_or_directory));
        }

        // adds filename to ftDir (implicitly casted to filesystem::path)
        auto fileName = (ftDir / detail::matchTypeToFileName(type));
        mFile.open(fileName.string(), cv::FileStorage::READ);

        if (!mFile.isOpened())
        {
            throw std::filesystem::filesystem_error("Error opening matches file",
                fileName, std::make_error_code(std::errc::io_error));
        }

        cv::read(mFile[detail::pairMatKey], mPairMat);
    }

    size_t size() const override { return mPairMat.rows; }

    std::vector<cv::DMatch> get(const std::pair<size_t, size_t>& idPair) const override
    {
        std::vector<cv::DMatch> matches;
        cv::read(mFile[detail::idToMatchKey(idPair.first, idPair.second)], matches);
        return matches;
    }

};
} // namespace detail
} // namespace cvutils

#endif // CVUTILS_FEATURE_FETCHER_H
