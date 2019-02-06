#include "featureDetector.h"

#include <fstream>
#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include "tqdm/tqdm.h"

namespace cvutils
{
FeatureDetector::FeatureDetector(const std::string& imgDir,
    const std::string& txtFile, const std::string& ftDir,
    const std::string& ftFile, float scale)
    : mReader(imgDir, txtFile, scale, cv::ImreadModes::IMREAD_GRAYSCALE, 0)
    , mFtWriter(ftDir)
    , mDescWriter(ftDir)
    , mFtFile(ftFile)
{
    if (!std::filesystem::exists(mFtFile)
        || !std::filesystem::is_regular_file(mFtFile))
    {
        throw std::filesystem::filesystem_error("Feature file does not exist", mFtFile,
            std::make_error_code(std::errc::no_such_file_or_directory));
    }
}

void FeatureDetector::run()
{
    auto ftPtr = getFtPtr();
    //auto files = misc::IO::getImgFiles(mInFolder, mTxtFile);
    auto numFiles = mReader.numImages();

    tqdm bar;
    size_t current = 0;
    // NOTE: parallel works only in this case
    // this underlying storage does not cache and is thus thread safe
    #pragma omp parallel for
    for (size_t i = 0; i < numFiles; i++)
    {
        auto img = mReader.getImage(i);
        std::vector<cv::KeyPoint> features;
        cv::Mat descriptors;
        ftPtr->detectAndCompute(img, cv::Mat(), features, descriptors);
        mFtWriter.writeFeatures(mReader.getImageName(i), features);
        mDescWriter.writeDescriptors(mReader.getImageName(i), descriptors);

        #pragma omp critical
        bar.progress(current++, numFiles);
    }
}

cv::Ptr<cv::Feature2D> FeatureDetector::getFtPtr()
{
    cv::FileStorage fs;
    fs.open(mFtFile, cv::FileStorage::READ);

    std::string name = fs["name"];
    cv::Ptr<cv::Feature2D> ftPtr;
    if (name == "orb")
        ftPtr = getORBPtr(fs);
    else if (name == "sift")
        ftPtr = getSIFTPtr(fs);
    else if (name == "surf")
        ftPtr = getSURFPtr(fs);

    return ftPtr;
}

cv::Ptr<cv::Feature2D> FeatureDetector::getORBPtr(const cv::FileStorage& fs)
{
    auto nf = static_cast<int>(fs["nfeatures"]);
    auto sf = static_cast<float>(fs["scaleFactor"]);
    auto nl = static_cast<int>(fs["nlevels"]);
    auto et = static_cast<int>(fs["edgeThreshold"]);
    auto fl = static_cast<int>(fs["firstLevel"]);
    auto wta = static_cast<int>(fs["WTA_K"]);
    auto st = static_cast<std::string>(fs["scoreType"]);
    auto ps = static_cast<int>(fs["patchSize"]);
    auto ft = static_cast<int>(fs["fastThreshold"]);

    cv::ORB::ScoreType score;
    if (st == "harris")
        score = cv::ORB::HARRIS_SCORE;
    else
        score = cv::ORB::FAST_SCORE;

    return cv::ORB::create(nf, sf, nl, et, fl, wta, score, ps, ft);
}

cv::Ptr<cv::Feature2D> FeatureDetector::getSIFTPtr(const cv::FileStorage& fs)
{
    auto nf = static_cast<int>(fs["nfeatures"]);
    auto no = static_cast<int>(fs["nOctaveLayers"]);
    auto ct = static_cast<double>(fs["contrastThreshold"]);
    auto et = static_cast<double>(fs["edgeThreshold"]);
    auto s = static_cast<double>(fs["sigma"]);

    return cv::xfeatures2d::SIFT::create(nf, no, ct, et, s);
}
cv::Ptr<cv::Feature2D> FeatureDetector::getSURFPtr(const cv::FileStorage& fs)
{
    auto hess = static_cast<double>(fs["hessianThreshold"]);
    auto nOct = static_cast<int>(fs["nOctaves"]);
    auto nOctL = static_cast<int>(fs["nOctaveLayers"]);

    // this mess is needed, because OpenCV does not implement
    // conversion to bool (only to int, double, string and float)
    auto ext = static_cast<int>(fs["extended"]);
    auto up = static_cast<int>(fs["up"]);

    bool extB = false;
    bool upB = false;
    if (ext)
        extB = true;
    if (up)
        upB = true;

    return cv::xfeatures2d::SURF::create(hess, nOct, nOctL, extB, upB);

    
}
}
