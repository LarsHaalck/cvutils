#ifndef CVUTILS_DESCRIPTOR_READER_H
#define CVUTILS_DESCRIPTOR_READER_H

#include <filesystem>
#include <memory>
#include <vector>

#include <opencv2/core.hpp>

#include "cache/abstractCache.h"
#include "fetch/descriptorFetcher.h"

namespace cvutils
{
class DescriptorReader
{
private:
    std::shared_ptr<detail::DescriptorFetcher> mFetcher;
    size_t mSize;
    std::unique_ptr<detail::AbstractCache<size_t, cv::Mat>> mCache;
public:
    DescriptorReader(const std::filesystem::path& imgDir,
        const std::filesystem::path& txtFile, const std::filesystem::path& ftDir,
        int cacheSize = 0);

    size_t numImages() { return mSize; }
    cv::Mat getDescriptors(size_t idx) { return mCache->get(idx); }
};
} // namespace cvutils

#endif // CVUTILS_DESCRIPTOR_READER_H
