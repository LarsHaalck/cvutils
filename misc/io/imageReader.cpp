#include "imageReader.h"

#include "fetch/imageFetcher.h"
#include "cache/simpleCache.h"
#include "cache/lruCache.h"

namespace cvutils
{
namespace io
{
ImageReader::ImageReader(const std::filesystem::path& imgDir,
    const std::filesystem::path& txtFile, size_t cacheSize, cv::ImreadModes mode)
{
    using namespace cache;
    using namespace fetch;

    auto imgFetcher = std::make_unique<ImageFetcher>(imgDir, txtFile, mode);
    mSize = imgFetcher->size();

    if (cacheSize)
    {
        mCache = std::make_unique<LRUCache<size_t, cv::Mat>>(cacheSize,
            std::move(imgFetcher));
    }
    else
    {
        mCache = std::make_unique<SimpleCache<size_t, cv::Mat>>(std::move(imgFetcher));
        // this resizes the underlying hash map to improve performance
        mCache->setCapacityHint(imgFetcher->size());
    }
}

} // namespace io
} // namespace cvutils
