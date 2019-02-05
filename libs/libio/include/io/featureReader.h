#ifndef CVUTILS_IO_FEATURE_READER_H
#define CVUTILS_IO_FEATURE_READER_H

#include <filesystem>
#include <memory>
#include <vector>

#include <opencv2/core.hpp>

#include "cache/abstractCache.h"
#include "fetch/featureFetcher.h"


namespace cvutils
{
namespace io
{
class FeatureReader
{
private:
    std::shared_ptr<fetch::FeatureFetcher> mFetcher;
    size_t mSize;
    std::unique_ptr<cache::AbstractCache<size_t, std::vector<cv::KeyPoint>>> mCache;
public:
    FeatureReader(const std::filesystem::path& imgDir,
        const std::filesystem::path& txtFile, const std::filesystem::path& ftDir,
        size_t cacheSize = 0);

    size_t numImages() { return mSize; }
    std::vector<cv::KeyPoint> getFeatures(size_t idx) { return mCache->get(idx); }
};
} // namespace io
} // namespace cvutils


#endif // CVUTILS_IO_FEATURE_READER_H
