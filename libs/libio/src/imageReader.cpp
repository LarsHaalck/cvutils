#include "io/imageReader.h"


#include "fetch/imageFetcher.h"
#include "cache/cacheFactory.h"

namespace cvutils
{
ImageReader::ImageReader(const std::filesystem::path& imgDir,
    const std::filesystem::path& txtFile, float scale, cv::ImreadModes mode,
    size_t cacheSize)
    : mFetcher(std::make_shared<detail::ImageFetcher>(imgDir, txtFile, scale, mode))
    , mSize(mFetcher->size())
    , mCache(detail::createCachePtr<size_t, cv::Mat>(mFetcher, cacheSize))
{
}
} // namespace cvutils
