#ifndef CVUTILS_IO_IMAGE_READER_H
#define CVUTILS_IO_IMAGE_READER_H

#include <filesystem>
#include <memory>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "cache/abstractCache.h"


namespace cvutils
{
namespace io
{
class ImageReader
{
private:
    std::unique_ptr<cache::AbstractCache<size_t, cv::Mat>> mCache;
    size_t mSize;
public:
    ImageReader(const std::filesystem::path& imgDir,
        const std::filesystem::path& txtFile, size_t cacheSize = 0,
        cv::ImreadModes mode = cv::ImreadModes::IMREAD_UNCHANGED);

    size_t numImages() { return mSize; }
    cv::Mat getImage(size_t idx) { return mCache->get(idx); }
};
} // namespace io
} // namespace cvutils


#endif // CVUTILS_IO_IMAGE_READER_H
