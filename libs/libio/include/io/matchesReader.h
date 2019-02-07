#ifndef CVUTILS_MATCHES_READER_H
#define CVUTILS_MATCHES_READER_H

#include <filesystem>
#include <memory>
#include <vector>

#include <opencv2/core.hpp>

#include "cache/abstractCache.h"
#include "fetch/matchesFetcher.h"

namespace cvutils
{
class MatchesReader
{
private:
    std::shared_ptr<detail::MatchesFetcher> mFetcher;
    size_t mSize;
    std::unique_ptr<detail::AbstractCache<std::pair<size_t, size_t>,
        std::vector<cv::DMatch>>> mCache;
public:
    MatchesReader(const std::filesystem::path& ftDir, MatchType type,
        int cacheSize = 0);

    size_t numMatches() const;
    std::vector<cv::DMatch> getMatches(size_t idI, size_t idJ);
    cv::Mat getPairMat() const;
};
} // namespace cvutils

#endif // CVUTILS_MATCHES_READER_H
