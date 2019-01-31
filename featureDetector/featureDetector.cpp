#include "featureDetector.h"

#include <fstream>
#include <iostream>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "io.h"
#include "tqdm.h"


namespace cvutils
{
FeatureDetector::FeatureDetector(const std::string& inFolder,
    const std::string& outFolder, const std::string& txtFile,
    const std::string& ftFile, float scale)
    : mInFolder(inFolder)
    , mOutFolder(outFolder)
    , mHasTxtFile(!txtFile.empty())
    , mTxtFile(txtFile)
    , mFtFile(ftFile)
    , mScale(scale)
{

    if (!std::filesystem::exists(mInFolder)
        || !std::filesystem::is_directory(mInFolder))
    {
        throw std::filesystem::filesystem_error("Input folder does not exist",
            mInFolder, std::make_error_code(std::errc::no_such_file_or_directory));
    }

    std::filesystem::create_directory(mOutFolder);

    if (mHasTxtFile && (!std::filesystem::exists(mTxtFile)
        || !std::filesystem::is_regular_file(mTxtFile)))
    {
        throw std::filesystem::filesystem_error("Txt file does not exist", mTxtFile,
            std::make_error_code(std::errc::no_such_file_or_directory));
    }

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
    auto files = misc::getImgFiles(mInFolder, mTxtFile);

    tqdm bar;
    size_t current = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < files.size(); i++)
    {
        std::string file = files[i];
        //std::cout << "Processing file: " << file << std::endl;
        cv::Mat img = cv::imread(file, cv::IMREAD_GRAYSCALE);

        if (mScale != 1.0f)
        {
            cv::Mat resImg;
            cv::resize(img, resImg, cv::Size(0, 0), mScale, mScale);
            img = resImg;
        }

        std::vector<cv::KeyPoint> features;
        ftPtr->detect(img, features);

        cv::Mat descriptors;
        ftPtr->compute(img, features, descriptors);

        std::filesystem::path imgStem(file);
        imgStem = mOutFolder / imgStem.stem();
        cv::FileStorage fsFt(imgStem.string() + "-feat.yml.gz",
            cv::FileStorage::WRITE);
        //std::cout << "writing to " << imgStem.string() + "-feat.yml" << std::endl;

        if (!fsFt.isOpened())
            std::cout << "Could not open feature file for writing" << std::endl;
        else
            cv::write(fsFt, "pts", features);

        cv::FileStorage fsDesc(imgStem.string() + "-desc.yml.gz",
            cv::FileStorage::WRITE);
        if (!fsDesc.isOpened())
            std::cout << "Could not open descriptor file for writing" << std::endl;
        else
            cv::write(fsDesc, "desc", descriptors);

        #pragma omp critical
        bar.progress(current++, files.size());
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
