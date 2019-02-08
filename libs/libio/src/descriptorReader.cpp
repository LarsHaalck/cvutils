#include "io/descriptorReader.h"

#include "cache/cacheFactory.h"

namespace cvutils
{
DescriptorReader::DescriptorReader(const std::filesystem::path& imgDir,
    const std::filesystem::path& txtFile, const std::filesystem::path& ftDir,
    int cacheSize)
    : mFetcher(std::make_shared<detail::DescriptorFetcher>(imgDir, txtFile, ftDir))
    , mSize(mFetcher->size())
    , mCache(detail::createCachePtr<size_t, cv::Mat>(mFetcher, cacheSize))
{

}
} // namespace cvutils
