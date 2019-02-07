#ifndef CVUTILS_IMAGE_READER_H
#define CVUTILS_IMAGE_READER_H

#include <filesystem>
#include <memory>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "cache/abstractCache.h"
#include "fetch/imageFetcher.h"

namespace cvutils
{
class ImageReader
{
private:
    std::shared_ptr<detail::ImageFetcher> mFetcher;
    size_t mSize;
    std::unique_ptr<detail::AbstractCache<size_t, cv::Mat>> mCache;
public:
    ImageReader(const std::filesystem::path& imgDir,
        const std::filesystem::path& txtFile, float scale = 1.0f,
        cv::ImreadModes mode = cv::ImreadModes::IMREAD_UNCHANGED, int cacheSize = 0);

    size_t numImages() { return mSize; }
    cv::Mat getImage(size_t idx) { return mCache->get(idx); }
    std::string getImageName(size_t idx) { return mFetcher->getImageName(idx); }
    std::string getImagePath(size_t idx) { return mFetcher->getImagePath(idx); }
};
} // namespace cvutils


#endif // CVUTILS_IMAGE_READER_H
