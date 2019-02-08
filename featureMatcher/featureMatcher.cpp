#include "featureMatcher.h"

#include <iostream>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include "tqdm/tqdm.h"

namespace cvutils
{
    FeatureMatcher::FeatureMatcher(
        const std::filesystem::path& imgFolder,
        const std::filesystem::path& txtFile,
        const std::filesystem::path& ftDir,
        int matcher, int window, int cacheSize)
    : mImgFolder(imgFolder)
    , mTxtFile(txtFile)
    , mFtDir(ftDir)
    , mMatcher(matcher)
    , mWindow(window)
    , mCacheSize(cacheSize)
{

}

void FeatureMatcher::run()
{
    getPutativeMatches();
    getGeomMatches();
}

void FeatureMatcher::getPutativeMatches()
{
    DescriptorReader descReader(mImgFolder, mTxtFile, mFtDir, mCacheSize);
    MatchesWriter matchesWriter(mFtDir, MatchType::Putative);

    auto pairList = getPairList(descReader.numImages());
    auto descMatcher = getMatcher();

    // NOTE: opencv doesnt support CV_32U
    cv::Mat pairMat = cv::Mat::zeros(pairList.size(), 2, CV_32S);
    std::vector<size_t> matchSizes(pairList.size());

    std::cout << "\nPutative matching..." << std::endl;
    tqdm bar;
    size_t count = 0;
    #pragma omp parallel for
    for (size_t k = 0; k < pairList.size(); k++)
    {
        auto pair = pairList[k];
        size_t idI = pair.first;
        size_t idJ = pair.second;
        auto descI = descReader.getDescriptors(idI);
        auto descJ = descReader.getDescriptors(idJ);

        std::vector<cv::DMatch> currMatches;
        descMatcher->match(descI, descJ, currMatches);

        pairMat.at<int>(k, 0) = idI;
        pairMat.at<int>(k, 1) = idJ;
        matchSizes[k] = currMatches.size();

        #pragma omp critical
        {
            matchesWriter.writeMatches(idI, idJ, currMatches);
            bar.progress(count++, pairList.size());
        }
    }
    matchesWriter.writePairMat(pairMat, matchSizes);
}

void FeatureMatcher::getGeomMatches()
{
    MatchesReader matchesReader(mFtDir, MatchType::Putative, -1);
    MatchesWriter matchesWriter(mFtDir, MatchType::Geometric);
    FeatureReader featReader(mImgFolder, mTxtFile, mFtDir, mCacheSize);

    std::cout << "\nGeometric verification..." << std::endl;
    tqdm bar;
    size_t count = 0;
    auto pairMat = matchesReader.getPairMat();
    std::vector<size_t> matchSizes(pairMat.rows);
    // for every matching image pair
    #pragma omp parallel for
    for (int k = 0; k < pairMat.rows; k++)
    {
        auto idI = pairMat.at<int>(k, 0);
        auto idJ = pairMat.at<int>(k, 1);
        auto matches = matchesReader.getMatches(idI, idJ);
        auto featsI = featReader.getFeatures(idI);
        auto featsJ = featReader.getFeatures(idJ);

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

        matches = filteredMatches;
        matchSizes[k] = filteredMatches.size();

        #pragma omp critical
        {
            matchesWriter.writeMatches(idI, idJ, matches);
            bar.progress(count++, pairMat.rows);
        }

    }
    matchesWriter.writePairMat(pairMat, matchSizes);
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
