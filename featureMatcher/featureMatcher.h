#ifndef CVUTILS_FEATURE_MATCHER_H
#define CVUTILS_FEATURE_MATCHER_H

#include <filesystem>
#include <string>
#include <utility>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "io/imageReader.h"
#include "io/featureReader.h"
#include "io/descriptorReader.h"
#include "io/matchesReader.h"
#include "io/matchesWriter.h"
#include "io/geometricType.h"

namespace cvutils
{
class FeatureMatcher
{
private:
    std::filesystem::path mImgFolder;
    std::filesystem::path mTxtFile;
    std::filesystem::path mFtDir;
    bool mIsBinary;
    int mMatcher;
    cvutils::GeometricType mGeomTypes;
    int mWindow;
    int mCacheSize;
    bool mPrune;
    double mCondition;
    double mMinDist;
    double mMinCoverage;
    bool mCheckSymmetry;
    cv::Mat mCamMat;
    cv::Mat mCamMatInv;
    cv::Mat mDistCoeffs;

public:
    FeatureMatcher(const std::filesystem::path& imgFolder,
        const std::filesystem::path& txtFile, const std::filesystem::path& ftFolder,
        bool isBinary, int matcher, cvutils::GeometricType geomTypes, int window,
        int cacheSize, bool prune, double condition, double minDist, double minCoverage,
        bool checkSymmetry, const cv::Mat& camMat, const cv::Mat& distCoeffs);


    void run();

private:
    cvutils::GeometricType findNextBestModel(cvutils::GeometricType currType);
    std::vector<std::pair<size_t, size_t>> getPairList(size_t size);
    std::vector<std::pair<size_t, size_t>> getExhaustivePairList(size_t size);
    std::vector<std::pair<size_t, size_t>> getWindowPairList(size_t size);
    cv::Ptr<cv::DescriptorMatcher> getMatcher();


    std::vector<cv::DMatch> match(cv::Ptr<cv::DescriptorMatcher> descMatcher,
        const cv::Mat& descI, const cv::Mat& descJ);

    std::vector<cv::DMatch> keepSymmetricMatches(const std::vector<cv::DMatch>& matchesI,
        const std::vector<cv::DMatch>& matchesJ);

    void getPutativeMatches();
    void getGeomMatches(cvutils::GeometricType writeType,
        cvutils::GeometricType readType);

    void filterMatches(cvutils::GeometricType geomType);

    std::vector<uchar> getInlierMask(const std::vector<cv::Point2f>& src,
        const std::vector<cv::Point2f>& dst, cvutils::GeometricType type);
    std::vector<uchar> getInlierMaskIsometry(const std::vector<cv::Point2f>& src,
        const std::vector<cv::Point2f>& dst);
    std::vector<uchar> getInlierMaskSimilarity(const std::vector<cv::Point2f>& src,
        const std::vector<cv::Point2f>& dst);
    std::vector<uchar> getInlierMaskAffinity(const std::vector<cv::Point2f>& src,
        const std::vector<cv::Point2f>& dst);
    std::vector<uchar> getInlierMaskHomography(const std::vector<cv::Point2f>& src,
        const std::vector<cv::Point2f>& dst);
    std::vector<uchar> getInlierMaskFundamental(const std::vector<cv::Point2f>& src,
        const std::vector<cv::Point2f>& dst);

    inline size_t getInlierCount(const std::vector<uchar>& mask)
    {
        size_t count = 0;
        for (const auto id : mask)
        {
            if (id)
                count++;
        }
        return count;

    }
};
}

#endif // CVUTILS_FEATURE_MATCHER_H
