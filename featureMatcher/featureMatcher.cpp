#include "featureMatcher.h"

#include <iostream>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "io.h"


namespace cvutils
{
    FeatureMatcher::FeatureMatcher(
        const std::filesystem::path& imgFolder,
        const std::filesystem::path& txtFile,
        const std::filesystem::path& ftFolder, int matcher,
        int window)
    : mHasTxtFile(!txtFile.string().empty())
    , mImgFolder(imgFolder)
    , mTxtFile(txtFile)
    , mFtFolder(ftFolder)
    , mMatcher(matcher)
    , mWindow(window)
{

    if (!std::filesystem::exists(mImgFolder)
        || !std::filesystem::is_directory(mImgFolder))
    {
        throw std::filesystem::filesystem_error("Img folder does not exist",
            mImgFolder, std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (mHasTxtFile && (!std::filesystem::exists(mTxtFile)
        || !std::filesystem::is_regular_file(mTxtFile)))
    {
        throw std::filesystem::filesystem_error("Txt file does not exist", mTxtFile,
            std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (!std::filesystem::exists(mFtFolder)
        || !std::filesystem::is_directory(mFtFolder))
    {
        throw std::filesystem::filesystem_error("Ft folder does not exist",
            mFtFolder, std::make_error_code(std::errc::no_such_file_or_directory));
    }
}

void FeatureMatcher::run()
{
    auto imgList = misc::getImgFiles(mImgFolder, mTxtFile);
    auto pairList = getPairList(imgList.size());


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
    //for (size_t i = 0; i < 
}

std::vector<std::pair<size_t, size_t>> FeatureMatcher::getExhaustivePairList(size_t size)
{
}
}
