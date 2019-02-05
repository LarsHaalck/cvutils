#include "io/featureReader.h"

#include "cache/cacheFactory.h"

using KeyPoints = std::vector<cv::KeyPoint>;

namespace cvutils
{
namespace io
{
FeatureReader::FeatureReader(const std::filesystem::path& imgDir,
    const std::filesystem::path& txtFile, const std::filesystem::path& ftDir,
    size_t cacheSize)
    : mFetcher(std::make_shared<fetch::FeatureFetcher>(imgDir, txtFile, ftDir))
    , mSize(mFetcher->size())
    , mCache(cvutils::cache::createCachePtr<size_t, KeyPoints>(mFetcher, cacheSize))
{
}

} // namespace io
} // namespace cvutils
