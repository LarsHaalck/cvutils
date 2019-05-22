#include "featureMatcher.h"

#include <iostream>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include "transformation/isometry.h"
#include "tqdm.h"

namespace cvutils
{
    FeatureMatcher::FeatureMatcher(
    const std::filesystem::path& imgFolder,
    const std::filesystem::path& txtFile,
    const std::filesystem::path& ftDir,
    bool isBinary, int matcher, cvutils::GeometricType geomTypes, int window,
    int cacheSize, bool prune, double condition, double minDist, double minCoverage)
    : mImgFolder(imgFolder)
    , mTxtFile(txtFile)
    , mFtDir(ftDir)
    , mIsBinary(isBinary)
    , mMatcher(matcher)
    , mGeomTypes(geomTypes)
    , mWindow(window)
    , mCacheSize(cacheSize)
    , mPrune(prune)
    , mCondition(condition)
    , mMinDist(minDist)
    , mMinCoverage(minCoverage)
{
    mGeomTypes |= cvutils::GeometricType::Putative;
}

void FeatureMatcher::run()
{
    getPutativeMatches();

    auto hom = cvutils::GeometricType::Homography;
    auto aff = cvutils::GeometricType::Affinity;
    auto sim = cvutils::GeometricType::Similarity;
    auto iso = cvutils::GeometricType::Isometry;

    if (static_cast<unsigned int>(mGeomTypes & hom))
    {
        getGeomMatches(hom, findNextBestModel(hom));
        filterMatches(hom);
    }

    if (static_cast<unsigned int>(mGeomTypes & aff))
    {
        getGeomMatches(aff, findNextBestModel(aff));
        filterMatches(aff);
    }

    if (static_cast<unsigned int>(mGeomTypes & sim))
    {
        getGeomMatches(sim, findNextBestModel(sim));
        filterMatches(sim);
    }

    if (static_cast<unsigned int>(mGeomTypes & iso))
    {
        getGeomMatches(iso, findNextBestModel(iso));
        filterMatches(iso);
    }
}

cvutils::GeometricType FeatureMatcher::findNextBestModel(
    cvutils::GeometricType currType)
{
    using geom = cvutils::GeometricType;
    switch (currType)
    {
        case geom::Isometry:
            if (static_cast<unsigned int>(mGeomTypes & geom::Similarity))
                return geom::Similarity;
            [[fallthrough]];
        case geom::Similarity:
            if (static_cast<unsigned int>(mGeomTypes & geom::Affinity))
                return geom::Affinity;
            [[fallthrough]];
        case geom::Affinity:
            if (static_cast<unsigned int>(mGeomTypes & geom::Homography))
                return geom::Homography;
            [[fallthrough]];
        case geom::Homography:
            return geom::Putative;
        default:
            return geom::Undefined;
    }
}

void FeatureMatcher::getPutativeMatches()
{
    DescriptorReader descReader(mImgFolder, mTxtFile, mFtDir, mCacheSize);
    MatchesWriter matchesWriter(mFtDir, GeometricType::Putative);

    auto pairList = getPairList(descReader.numImages());
    auto descMatcher = getMatcher();

    std::cout << "\nPutative matching..." << std::endl;
    tqdm bar;
    size_t count = 0;
    // opencv uses parallelization internally
    // openmp for loop is not necessary and in this case even performance hindering
    #pragma omp parallel for schedule(dynamic, 2)
    for (size_t k = 0; k < pairList.size(); k++)
    {
        const auto pair = pairList[k];
        size_t idI = pair.first;
        size_t idJ = pair.second;
        const auto descI = descReader.getDescriptors(idI);
        const auto descJ = descReader.getDescriptors(idJ);

        std::vector<cv::DMatch> currMatches;
        if (mMinDist > 0)
        {
            std::vector<std::vector<cv::DMatch>> knnMatches;
            descMatcher->knnMatch(descI, descJ, knnMatches, 2);

            for (size_t i = 0; i < knnMatches.size(); ++i)
            {
                if (knnMatches[i][0].distance < mMinDist * knnMatches[i][1].distance)
                    currMatches.push_back(knnMatches[i][0]);
            }

        }
        else
            descMatcher->match(descI, descJ, currMatches);


        #pragma omp critical
        {
            matchesWriter.writeMatches(idI, idJ, std::move(currMatches));
            bar.progress(count++, pairList.size());
        }
    }
}

void FeatureMatcher::getGeomMatches(cvutils::GeometricType writeType,
    cvutils::GeometricType readType)
{
    auto pairwiseMatches = MatchesReader(mFtDir, readType).moveMatches();
    MatchesWriter matchesWriter(mFtDir, writeType);
    FeatureReader featReader(mImgFolder, mTxtFile, mFtDir, mCacheSize);
    // TODO: scale
    ImageReader imgReader(mImgFolder, mTxtFile);

    std::cout << "\nGeometric verification..." << std::endl;
    tqdm bar;
    size_t count = 0;

    std::vector<std::pair<size_t, size_t>> keys;
    keys.reserve(pairwiseMatches.size());

    for(const auto& matches : pairwiseMatches)
        keys.push_back(matches.first);

    // for every matching image pair
    #pragma omp parallel for schedule(dynamic, 2)
    for (size_t i = 0; i < keys.size(); i++)
    {
        auto pair = keys[i];
        auto idI = pair.first;
        auto idJ = pair.second;
        auto matches = std::move(pairwiseMatches[pair]);
        const auto featsI = featReader.getFeatures(idI);
        const auto featsJ = featReader.getFeatures(idJ);

        //build point matrices
        std::vector<cv::Point2f> src, dst;
        for (size_t i = 0; i < matches.size(); i++)
        {
            src.push_back(featsI[matches[i].queryIdx].pt);
            dst.push_back(featsJ[matches[i].trainIdx].pt);
        }

        auto mask = getInlierMask(src, dst, writeType);
        std::vector<cv::DMatch> filteredMatches;
        std::vector<cv::Point2f> srcFiltered, dstFiltered;
        for (size_t r = 0; r < mask.size(); r++)
        {
            if (mask[r])
            {
               filteredMatches.push_back(matches[r]);
               srcFiltered.push_back(src[r]);
               dstFiltered.push_back(dst[r]);
            }
        }

        if (mMinCoverage)
        {
            int rectI = cv::boundingRect(srcFiltered).area();
            int rectJ = cv::boundingRect(dstFiltered).area();
            int areaI = imgReader.getImage(idI).rows * imgReader.getImage(idI).cols;
            int areaJ = imgReader.getImage(idJ).rows * imgReader.getImage(idJ).cols;
            if (rectI < mMinCoverage * areaI || rectJ < mMinCoverage * areaJ)
                filteredMatches.clear();

        }

        matches = std::move(filteredMatches);

        #pragma omp critical
        {
            matchesWriter.writeMatches(idI, idJ, std::move(matches));
            bar.progress(count++, keys.size());
        }
    }

}

void FeatureMatcher::filterMatches(cvutils::GeometricType geomType)
{
    // this only makes sense for window matching
    if (mWindow == 0 || !mPrune)
        return;

    auto pairwiseMatches = MatchesReader(mFtDir, geomType).moveMatches();
    MatchesWriter matchesWriter(mFtDir, geomType);

    std::vector<std::pair<size_t, size_t>> keys;
    keys.reserve(pairwiseMatches.size());

    for(const auto& matches : pairwiseMatches)
    {
        if (!matches.second.empty())
            keys.push_back(matches.first);
    }

    std::sort(std::begin(keys), std::end(keys));
    std::pair<size_t, size_t> lastPair = keys[0];
    std::pair<size_t, size_t> drain = {-1, -1};
    for (size_t i = 1; i < keys.size(); i++)
    {
        // check if gap is more than 2
        if (lastPair.first == keys[i].first && lastPair.second + 2 < keys[i].second)
           drain = keys[i];

        if (drain.first != static_cast<size_t>(-1))
        {
            if (keys[i].first == drain.first && keys[i].second >= drain.second)
                pairwiseMatches.erase(keys[i]);
            else
                drain.first = -1;
        }
        lastPair = keys[i];
    }

    matchesWriter.writePairWiseMatches(std::move(pairwiseMatches));
}

std::vector<uchar> FeatureMatcher::getInlierMask(const std::vector<cv::Point2f>& src,
    const std::vector<cv::Point2f>& dst, cvutils::GeometricType type)
{
    std::vector<uchar> mask(src.size(), 0);
    switch (type)
    {
        case cvutils::GeometricType::Isometry:
            return getInlierMaskIsometry(src, dst);
        case cvutils::GeometricType::Similarity:
            return getInlierMaskSimilarity(src, dst);
        case cvutils::GeometricType::Affinity:
            return getInlierMaskAffinity(src, dst);
        case cvutils::GeometricType::Homography:
            return getInlierMaskHomography(src, dst);
        default:
            return mask;
    }
    return mask;
}
std::vector<uchar> FeatureMatcher::getInlierMaskIsometry(
    const std::vector<cv::Point2f>& src, const std::vector<cv::Point2f>& dst)
{
    std::vector<uchar> mask;
    if (src.size() >= 2)
        cv::estimateIsometry2D(src, dst, mask, cv::RANSAC);
    else
        mask = std::vector<uchar>(src.size(), 0);

    return mask;
}

std::vector<uchar> FeatureMatcher::getInlierMaskSimilarity(
    const std::vector<cv::Point2f>& src, const std::vector<cv::Point2f>& dst)
{
    cv::Mat mat;
    std::vector<uchar> mask;
    if (src.size() >= 2)
        mat = cv::estimateAffinePartial2D(src, dst, mask, cv::RANSAC);
    else
        mask = std::vector<uchar>(src.size(), 0);

    return mask;
}

std::vector<uchar> FeatureMatcher::getInlierMaskAffinity(
    const std::vector<cv::Point2f>& src, const std::vector<cv::Point2f>& dst)
{
    cv::Mat mat;
    std::vector<uchar> mask;
    if (src.size() >= 3)
        mat = cv::estimateAffine2D(src, dst, mask, cv::RANSAC);
    else
        mask = std::vector<uchar>(src.size(), 0);

    return mask;
}


std::vector<uchar> FeatureMatcher::getInlierMaskHomography(const std::vector<cv::Point2f>& src,
    const std::vector<cv::Point2f>& dst)
{
    std::vector<uchar> mask;
    cv::Mat mat;
    if (src.size() >= 10)
        mat = cv::findHomography(src, dst, mask, cv::RANSAC);
    else
        return std::vector<uchar>(src.size(), 0);


    if (mCondition && !mat.empty())
    {
        cv::SVD svd(mat, cv::SVD::NO_UV);
        double conditionNumber = svd.w.at<double>(0, 1) / svd.w.at<double>(2, 0);
        /* std::cout << conditionNumber << std::endl; */
        if (conditionNumber > mCondition)
            return std::vector<uchar>(src.size(), 0);
    }

    return mask;
}

std::vector<std::pair<size_t, size_t>> FeatureMatcher::getPairList(size_t size)
{
    if (mWindow != 0)
        return getWindowPairList(size);
    else
        return getExhaustivePairList(size);
}

std::vector<std::pair<size_t, size_t>> FeatureMatcher::getWindowPairList(size_t size)
{
    std::vector<std::pair<size_t, size_t>> pairList;
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = i + 1; (j < i + mWindow) && (j < size); j++)
        {
            pairList.push_back(std::make_pair(i, j));
        }
    }
    return pairList;
}

std::vector<std::pair<size_t, size_t>> FeatureMatcher::getExhaustivePairList(size_t size)
{
    std::vector<std::pair<size_t, size_t>> pairList;
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = i + 1; j < size; j++)
        {
            pairList.push_back(std::make_pair(i, j));
        }
    }
    return pairList;
}

cv::Ptr<cv::DescriptorMatcher> FeatureMatcher::getMatcher()
{
    if (mMatcher)
    {
        if (mIsBinary)
        {
            return cv::makePtr<cv::FlannBasedMatcher>(cv::makePtr<cv::flann::LshIndexParams>(20, 10, 2));
        }
        else
        {
            return cv::DescriptorMatcher::create(
                cv::DescriptorMatcher::MatcherType::FLANNBASED);
        }
    }
    else
    {
        return cv::DescriptorMatcher::create(
            cv::DescriptorMatcher::MatcherType::BRUTEFORCE);

    }
}

}
