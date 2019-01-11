#include "featureDetector.h"

#include <iostream>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgcodecs.hpp>

namespace cvutils
{
FeatureDetector::FeatureDetector(const std::string& inFolder,
        const std::string& outFolder, const std::string& ftFile)
    : mInFolder(inFolder)
      , mOutFolder(outFolder)
      , mFtFile(ftFile)
{

}

void FeatureDetector::run()
{
    auto ftPtr = getFtPtr();

    std::vector<std::string> files;
    cv::glob(mInFolder, files, false);
    for (const auto& file : files)
    {
        std::cout << "Processing file: " << file << std::endl;
        cv::Mat img = cv::imread(file, cv::IMREAD_GRAYSCALE);
        std::vector<cv::KeyPoint> features;
        ftPtr->detect(img, features);

        std::cout << features.size() << std::endl;

        cv::Mat descriptors;
        ftPtr->compute(img, features, descriptors);

        std::string fileName = file.substr(0, file.find_last_of("."));
        auto lastSlash = fileName.find_last_of("/");
        if (lastSlash != std::string::npos)
            fileName = fileName.substr(lastSlash);

        cv::FileStorage fsFt(mOutFolder + '/' + fileName + "-feat.yml",
            cv::FileStorage::WRITE);
        std::cout << "writing to " << mOutFolder + '/' + fileName + "-feat.yml" << std::endl;
        if (!fsFt.isOpened())
        {
            std::cout << "Could not open" << std::endl;
        }
        cv::write(fsFt, "pts", features);

        cv::FileStorage fsDesc(mOutFolder + '/' + fileName + "-desc.yml",
            cv::FileStorage::WRITE);
        cv::write(fsDesc, "desc", descriptors);

    }
}
cv::Ptr<cv::Feature2D> FeatureDetector::getFtPtr()
{
    cv::FileStorage fs;
    fs.open(mFtFile, cv::FileStorage::READ);

    std::string name = fs["name"];
    cv::Ptr<cv::Feature2D> ftPtr;
    if (name == "orb")
    {
        ftPtr = getORBPtr(fs);
    }
    // TODO: support other fts

    return ftPtr;
}

cv::Ptr<cv::Feature2D> FeatureDetector::getORBPtr(const cv::FileStorage& fs)
{
    int nf = static_cast<int>(fs["nfeatures"]);
    float sf = static_cast<float>(fs["scaleFactor"]);
    int nl = static_cast<int>(fs["nlevels"]);
    int et = static_cast<int>(fs["edgeThreshold"]);
    int fl = static_cast<int>(fs["firstLevel"]);
    int wta = static_cast<int>(fs["WTA_K"]);
    std::string st = static_cast<std::string>(fs["scoreType"]);
    int ps = static_cast<int>(fs["patchSize"]);
    int ft = static_cast<int>(fs["fastThreshold"]);

    cv::ORB::ScoreType score;
    if (st == "harris")
        score = cv::ORB::HARRIS_SCORE;
    else
        score = cv::ORB::FAST_SCORE;

    return cv::ORB::create(nf, sf, nl, et, fl, wta, score, ps, ft);

}
}
