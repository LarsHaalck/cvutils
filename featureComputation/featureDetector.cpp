#include "featureDetector.h"

#include <fstream>
#include <iostream>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgcodecs.hpp>


namespace cvutils
{
FeatureDetector::FeatureDetector(const std::string& inFolder,
    const std::string& outFolder, const std::string& txtFile,
    const std::string& ftFile)
    : mInFolder(inFolder)
    , mOutFolder(outFolder)
    , mHasTxtFile(!txtFile.empty())
    , mTxtFile(txtFile)
    , mFtFile(ftFile)
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


    auto files = getImageFiles();
    for (const auto& file : files)
    {
        std::cout << "Processing file: " << file << std::endl;
        cv::Mat img = cv::imread(file, cv::IMREAD_GRAYSCALE);
        std::vector<cv::KeyPoint> features;
        ftPtr->detect(img, features);

        std::cout << features.size() << std::endl;

        cv::Mat descriptors;
        ftPtr->compute(img, features, descriptors);

        std::filesystem::path imgStem(file);
        imgStem = mOutFolder / imgStem.stem();
        cv::FileStorage fsFt(imgStem.string() + "-feat.yml",
            cv::FileStorage::WRITE);
        std::cout << "writing to " << imgStem.string() + "-feat.yml" << std::endl;
        if (!fsFt.isOpened())
        {
            std::cout << "Could not open" << std::endl;
        }
        cv::write(fsFt, "pts", features);

        cv::FileStorage fsDesc(imgStem.string() + "-desc.yml",
            cv::FileStorage::WRITE);
        cv::write(fsDesc, "desc", descriptors);
    }
}

std::vector<std::string> FeatureDetector::getImageFiles()
{
    std::vector<std::string> files;
    if (!mHasTxtFile)
        cv::glob(mInFolder.string(), files, false);
    else
    {
        std::ifstream stream(mTxtFile.string());
        std::string currLine;
        while(std::getline(stream, currLine))
        {
            if (!currLine.empty())
            {
                std::filesystem::path imgPath(currLine);
                files.push_back((mInFolder / imgPath).string());
            }
        }
    }

    return files;
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
