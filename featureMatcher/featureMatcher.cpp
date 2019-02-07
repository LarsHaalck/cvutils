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
    #ifdef _OPENMP
    getPutativeMatchesParallel();
    getGeomMatchesParallel();
    #else
    getPutativeMatches();
    getGeomMatches();
    #endif
}

void FeatureMatcher::getPutativeMatchesParallel()
{
    // disable cache
    DescriptorReader descReader(mImgFolder, mTxtFile, mFtDir, 0);
    MatchesWriter matchesWriter(mFtDir, MatchType::Putative);

    auto pairList = getPairList(descReader.numImages());
    auto descMatcher = getMatcher();

    // NOTE: opencv doesnt support CV_32U
    cv::Mat pairMat = cv::Mat::zeros(pairList.size(), 2, CV_32S);
    std::vector<size_t> matchSizes(pairList.size());

    int cacheSize = mCacheSize;
    int listSize = static_cast<int>(pairList.size());
    if (mCacheSize < 0)
        cacheSize = listSize;

    auto cache = std::vector<cv::Mat>(2 * cacheSize);
    int its = 1 + ((listSize - 1) / cacheSize);

    std::cout << "\nPutative matching..." << std::endl;
    tqdm bar;
    size_t count = 0 ;
    for (int i = 0; i < its ; i++)
    {
        auto globalStart = i * cacheSize;
        auto globalEnd = (i + 1) * cacheSize;
        if (globalEnd > listSize)
            globalEnd = listSize;

        auto localEnd = globalEnd - globalStart;

        // fill cache
        for (int k = 0, l = globalStart; k < localEnd; k++, l++)
        {
            auto pair = pairList[l];
            auto idI = pair.first;
            auto idJ = pair.second;
            cache[2*k] = descReader.getDescriptors(idI);
            cache[2*k + 1] = descReader.getDescriptors(idJ);
        }

        // match on cache[localStart, localEnd)
        std::vector<std::vector<cv::DMatch>> matches(localEnd);
        #pragma omp parallel for
        for (int k = 0; k < localEnd; k++)
        {
            std::vector<cv::DMatch> currMatches;
            descMatcher->match(cache[2*k], cache[2*k + 1], currMatches);
            matches[k] = currMatches;
        }

        count += localEnd;
        bar.progress(count, listSize);
        // write and fill pairMat
        for (int k = 0, l = globalStart; k < localEnd; k++, l++)
        {
            //matches[k] = currMatches;
            auto pair = pairList[l];
            auto idI = pair.first;
            auto idJ = pair.second;
            pairMat.at<int>(l, 0) = idI;
            pairMat.at<int>(l, 1) = idJ;
            matchSizes[l] = matches[k].size();
            matchesWriter.writeMatches(idI, idJ, matches[k]);
        }
    }
    matchesWriter.writePairMat(pairMat, matchSizes);

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

        matchesWriter.writeMatches(idI, idJ, currMatches);
        bar.progress(count++, pairList.size());
    }
    matchesWriter.writePairMat(pairMat, matchSizes);
}

void FeatureMatcher::getGeomMatchesParallel()
{
    MatchesReader matchesReader(mFtDir, MatchType::Putative, 0);
    MatchesWriter matchesWriter(mFtDir, MatchType::Geometric);
    FeatureReader featReader(mImgFolder, mTxtFile, mFtDir, 0);

    auto pairMat = matchesReader.getPairMat();
    std::vector<size_t> matchSizes(pairMat.rows);

    int cacheSize = mCacheSize;
    int listSize = static_cast<int>(pairMat.rows);
    if (mCacheSize < 0)
        cacheSize = listSize;

    auto cache0 = std::vector<std::vector<cv::KeyPoint>>(2 * cacheSize);
    auto cache1 = std::vector<std::vector<cv::DMatch>>(cacheSize);
    int its = 1 + ((listSize - 1) / cacheSize);

    std::cout << "\nGeometric matching..." << std::endl;
    tqdm bar;
    size_t count = 0 ;
    for (int i = 0; i < its ; i++)
    {
        auto globalStart = i * cacheSize;
        auto globalEnd = (i + 1) * cacheSize;
        if (globalEnd > listSize)
            globalEnd = listSize;

        auto localEnd = globalEnd - globalStart;

        // fill cache
        for (int k = 0, l = globalStart; k < localEnd; k++, l++)
        {
            auto idI = pairMat.at<int>(l, 0);
            auto idJ = pairMat.at<int>(l, 1);
            cache0[2*k] = featReader.getFeatures(idI);
            cache0[2*k + 1] = featReader.getFeatures(idJ);
            cache1[k] = matchesReader.getMatches(idI, idJ);
        }

        // match on cache[localStart, localEnd)
        std::vector<std::vector<cv::DMatch>> matches(localEnd);
        #pragma omp parallel for
        for (int k = 0; k < localEnd; k++)
        {
            auto& matches = cache1[k];
            const auto& featsI = cache0[2*k];
            const auto& featsJ = cache0[2*k + 1];
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
        }
        count += localEnd;
        bar.progress(count, listSize);

        // write and fill pairMat
        for (int k = 0, l = globalStart; k < localEnd; k++, l++)
        {
            //matches[k] = currMatches;
            auto idI = pairMat.at<int>(l, 0);
            auto idJ = pairMat.at<int>(l, 1);
            pairMat.at<int>(l, 0) = idI;
            pairMat.at<int>(l, 1) = idJ;
            matchSizes[l] = matches[k].size();
            matchesWriter.writeMatches(idI, idJ, matches[k]);
        }
    }
    matchesWriter.writePairMat(pairMat, matchSizes);
}
void FeatureMatcher::getGeomMatches()
{
    MatchesReader matchesReader(mFtDir, MatchType::Putative, mCacheSize);
    MatchesWriter matchesWriter(mFtDir, MatchType::Geometric);
    FeatureReader featReader(mImgFolder, mTxtFile, mFtDir, mCacheSize);

    std::cout << "\nGeometric verification..." << std::endl;
    tqdm bar;
    size_t count = 0;
    auto pairMat = matchesReader.getPairMat();
    std::vector<size_t> matchSizes(pairMat.rows);
    // for every matching image pair
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

        matchesWriter.writeMatches(idI, idJ, matches);
        bar.progress(count++, pairMat.rows);

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
