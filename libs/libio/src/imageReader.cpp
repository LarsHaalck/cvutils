#include "io/imageReader.h"


#include "fetch/imageFetcher.h"
#include "cache/cacheFactory.h"

namespace cvutils
{
namespace io
{
ImageReader::ImageReader(const std::filesystem::path& imgDir,
    const std::filesystem::path& txtFile, float scale, cv::ImreadModes mode,
    size_t cacheSize)
    : mFetcher(std::make_shared<fetch::ImageFetcher>(imgDir, txtFile, scale, mode))
    , mSize(mFetcher->size())
    , mCache(cvutils::cache::createCachePtr<size_t, cv::Mat>(mFetcher, cacheSize))
{
}

} // namespace io
} // namespace cvutils
