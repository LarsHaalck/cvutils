#ifndef CVUTILS_FEATURE_MATCHER_H
#define CVUTILS_FEATURE_MATCHER_H

#include <filesystem>
#include <string>
#include <utility>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "io/featureReader.h"
#include "io/descriptorReader.h"
#include "io/matchesReader.h"
#include "io/matchesWriter.h"

namespace cvutils
{
class FeatureMatcher
{
private:
    std::filesystem::path mImgFolder;
    std::filesystem::path mTxtFile;
    std::filesystem::path mFtDir;
    int mMatcher;
    int mWindow;
    int mCacheSize;
public:
    FeatureMatcher(const std::filesystem::path& imgFolder,
        const std::filesystem::path& txtFile, const std::filesystem::path& ftFolder,
        int matcher, int window, int cacheSize);
    void run();

private:
    //void test(const std::vector<std::string>& imgList);
    std::vector<std::pair<size_t, size_t>> getPairList(size_t size);
    std::vector<std::pair<size_t, size_t>> getExhaustivePairList(size_t size);
    std::vector<std::pair<size_t, size_t>> getWindowPairList(size_t size);
    cv::Ptr<cv::DescriptorMatcher> getMatcher();

    void getPutativeMatchesParallel();
    void getPutativeMatches();
    void getGeomMatchesParallel();
    void getGeomMatches();

    std::vector<uchar> getInlierMask(const std::vector<cv::Point2f>& src,
        const std::vector<cv::Point2f>& dst);
    std::vector<uchar> getInlierMaskHomo(const std::vector<cv::Point2f>& src,
        const std::vector<cv::Point2f>& dst);
};
}

#endif // CVUTILS_FEATURE_MATCHER_H
