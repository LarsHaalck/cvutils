#include "featureMatcher.h"

#include <iostream>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include "tqdm.h"

namespace cvutils
{
    FeatureMatcher::FeatureMatcher(
    const std::filesystem::path& imgFolder,
    const std::filesystem::path& txtFile,
    const std::filesystem::path& ftDir,
    int matcher, cvutils::GeometricType, int window, int cacheSize)
    : mImgFolder(imgFolder)
    , mTxtFile(txtFile)
    , mFtDir(ftDir)
    , mMatcher(matcher)
    , mGeomTypes(geomTypes)
    , mWindow(window)
    , mCacheSize(cacheSize)
{
    mGeomTypes |= cvutils::GeometricType::Putative;
}

void FeatureMatcher::run()
{
    getPutativeMatches();

    if (mGeomTypes & cvutils::GeometricType::Homography)
    {
        getGeomMatches(cvutils::GeometricType::Homography,
            findNextBestModel(cvutils::GeometricType::Homography, mGeomTypes));
    }
    if (mGeomTypes & cvutils::GeometricType::Affine)
    {
        getGeomMatches(cvutils::GeometricType::Affine,
            findNextBestModel(cvutils::GeometricType::Affine, mGeomTypes));
    }
    if (mGeomTypes & cvutils::GeometricType::Similarity)
    {
        getGeomMatches(cvutils::GeometricType::Similarity,
            findNextBestModel(cvutils::GeometricType::Similarity, mGeomTypes));
    }
    if (mGeomTypes & cvutils::GeometricType::Isometry)
    {
        getGeomMatches(cvutils::GeometricType::Isometry,
            findNextBestModel(cvutils::GeometricType::Isometry, mGeomTypes));
    }
}

void FeatureMatcher::getPutativeMatches()
{
    DescriptorReader descReader(mImgFolder, mTxtFile, mFtDir, mCacheSize);
    MatchesWriter matchesWriter(mFtDir, MatchType::Putative);

    auto pairList = getPairList(descReader.numImages());
    auto descMatcher = getMatcher();

    std::cout << "\nPutative matching..." << std::endl;
    tqdm bar;
    size_t count = 0;
    // opencv uses parallelization internally
    // openmp for loop is not necessary and in this case even performance hindering
    #pragma omp parallel for
    for (size_t k = 0; k < pairList.size(); k++)
    {
        const auto pair = pairList[k];
        size_t idI = pair.first;
        size_t idJ = pair.second;
        const auto descI = descReader.getDescriptors(idI);
        const auto descJ = descReader.getDescriptors(idJ);

        std::vector<cv::DMatch> currMatches;
        descMatcher->match(descI, descJ, currMatches);

        #pragma omp critical
        {
            matchesWriter.writeMatches(idI, idJ, std::move(currMatches));
            bar.progress(count++, pairList.size());
        }
    }
}

void FeatureMatcher::getGeomMatches(cvutils::GeometricType readType, 
    cvutils::GeometricType::writeType)
{
    auto pairwiseMatches = MatchesReader(mFtDir, MatchType::Putative).moveMatches();
    MatchesWriter matchesWriter(mFtDir, MatchType::Geometric);
    FeatureReader featReader(mImgFolder, mTxtFile, mFtDir, mCacheSize);

    std::cout << "\nGeometric verification..." << std::endl;
    tqdm bar;
    size_t count = 0;

    std::vector<std::pair<size_t, size_t>> keys;
    keys.reserve(pairwiseMatches.size());

    for(const auto& matches : pairwiseMatches)
        keys.push_back(matches.first);

    // for every matching image pair
    #pragma omp parallel for
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
        auto mask = getInlierMask(src, dst);
        std::vector<cv::DMatch> filteredMatches;
        for (size_t r = 0; r < mask.size(); r++)
        {
            if (mask[r])
               filteredMatches.push_back(matches[r]);
        }

        matches = std::move(filteredMatches);

        #pragma omp critical
        {
            matchesWriter.writeMatches(idI, idJ, std::move(matches));
            bar.progress(count++, keys.size());
        }
    }
}

std::vector<uchar> FeatureMatcher::getInlierMask(const std::vector<cv::Point2f>& src,
    const std::vector<cv::Point2f>& dst)
{
    return getInlierMaskHomo(src, dst);
}

std::vector<uchar> FeatureMatcher::getInlierMaskHomo(const std::vector<cv::Point2f>& src,
    const std::vector<cv::Point2f>& dst)
{
    std::vector<uchar> mask;
    if (src.size() >= 4)
        cv::findHomography(src, dst, cv::RANSAC, 3.0, mask);
    else
        mask = std::vector<uchar>(src.size(), 1);
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
        return cv::DescriptorMatcher::create(
            cv::DescriptorMatcher::MatcherType::FLANNBASED);
    }
    else
    {
        return cv::DescriptorMatcher::create(
            cv::DescriptorMatcher::MatcherType::BRUTEFORCE);

    }
}

}
