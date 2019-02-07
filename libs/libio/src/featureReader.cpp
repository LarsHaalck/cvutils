#include "io/featureReader.h"

#include "cache/cacheFactory.h"

using KeyPoints = std::vector<cv::KeyPoint>;

namespace cvutils
{
FeatureReader::FeatureReader(const std::filesystem::path& imgDir,
    const std::filesystem::path& txtFile, const std::filesystem::path& ftDir,
    int cacheSize)
    : mFetcher(std::make_shared<detail::FeatureFetcher>(imgDir, txtFile, ftDir))
    , mSize(mFetcher->size())
    , mCache(detail::createCachePtr<size_t, KeyPoints>(mFetcher, cacheSize))
{

}
} // namespace cvutils
