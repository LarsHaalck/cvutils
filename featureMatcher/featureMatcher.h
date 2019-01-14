#ifndef CVUTILS_FEATURE_MATCHER_H
#define CVUTILS_FEATURE_MATCHER_H

#include <filesystem>
#include <string>
#include <utility>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "io.h"

// forward declarations
namespace cv
{
    class Feature2D;
}

namespace cvutils
{
class FeatureMatcher
{
private:
    bool mHasTxtFile;
    std::filesystem::path  mImgFolder;
    std::filesystem::path  mTxtFile;
    std::filesystem::path  mFtFolder;
    int mMatcher;
    int mWindow;
public:
    FeatureMatcher(const std::filesystem::path& imgFolder,
        const std::filesystem::path& txtFile, const std::filesystem::path& ftFolder,
        int matcher, int window);
    void run();

private:
    //void test(const std::vector<std::string>& imgList);
    std::vector<std::pair<size_t, size_t>> getPairList(size_t size);
    std::vector<std::pair<size_t, size_t>> getExhaustivePairList(size_t size);
    std::vector<std::pair<size_t, size_t>> getWindowPairList(size_t size);
    cv::Ptr<cv::DescriptorMatcher> getMatcher();

    void write(const cv::Mat& pairMat,
        const std::vector<std::vector<cv::DMatch>>& matches, detail::MatchType type);


};
}

#endif // CVUTILS_FEATURE_MATCHER_H
