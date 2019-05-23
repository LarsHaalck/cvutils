#include "featureDetector.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include "tqdm.h"

namespace cvutils
{
FeatureDetector::FeatureDetector(const std::string& imgDir,
    const std::string& txtFile, const std::string& ftDir,
    const std::string& ftFile, float scale, bool removeDuplicates)
    : mReader(imgDir, txtFile, scale, cv::ImreadModes::IMREAD_GRAYSCALE, 0)
    , mFtWriter(ftDir)
    , mDescWriter(ftDir)
    , mFtFile(ftFile)
    , mRemoveDuplicates(removeDuplicates)
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
    auto numFiles = mReader.numImages();

    tqdm bar;
    size_t current = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < numFiles; i++)
    {
        auto img = mReader.getImage(i);
        std::vector<cv::KeyPoint> features;
        cv::Mat descriptors;
        ftPtr->detect(img, features);

        if (mRemoveDuplicates)
        {
            auto uniqueIds = getUniqueIds(features);
            std::vector<cv::KeyPoint> featuresFiltered;
            featuresFiltered.reserve(uniqueIds.size());

            for (size_t k = 0; k < uniqueIds.size(); k++)
                featuresFiltered.push_back(std::move(features[uniqueIds[k]]));
            features = std::move(featuresFiltered);
        }

        ftPtr->compute(img, features, descriptors);

        /* for (const auto& elem : features) */
        /*     std::cout << elem.pt.x << ", " << elem.pt.y << std::endl; */


        mFtWriter.writeFeatures(mReader.getImageName(i), features);
        mDescWriter.writeDescriptors(mReader.getImageName(i), descriptors);

        #pragma omp critical
        bar.progress(current++, numFiles);
    }
}

bool FeatureDetector::FeatureStruct::operator<(const FeatureStruct& rhs) const
{
    if (std::abs(this->x - rhs.x) < 1e-3)
    {
        if (std::abs(this->y - rhs.y) >= 1e-3 &&
            this->y < rhs.y)
        {
            return true;
        }
    }
    else
    {
        if (this->x < rhs.x)
            return true;
    }

    return false;
}

bool FeatureDetector::FeatureStruct::operator==(const FeatureStruct& rhs) const
{
    if (std::abs(this->x - rhs.x) < 1e-3 &&
        std::abs(this->y - rhs.y) < 1e-3)
    {
        return true;
    }
    return false;
}


std::vector<size_t> FeatureDetector::getUniqueIds(
    const std::vector<cv::KeyPoint>& features)
{
    std::vector<FeatureStruct> featuresStruct;
    featuresStruct.reserve(features.size());
    for (size_t i = 0; i < features.size(); i++)
        featuresStruct.push_back(FeatureStruct(features[i], i));

    // sort and erase non-unique elements
    std::sort(std::begin(featuresStruct), std::end(featuresStruct),
        [](const auto& lhs, const auto& rhs){ return lhs.operator<(rhs); });

    /* std::cout << "--------------------------------------------------------------------------------" << std::endl; */
    /* for (const auto& elem :  featuresStruct) */
    /*     std::cout << elem.x << ", " << elem.y << std::endl; */

    auto lastIt = std::unique(std::begin(featuresStruct), std::end(featuresStruct),
        [](const auto& lhs, const auto& rhs) { return lhs.operator==(rhs); });
    featuresStruct.erase(lastIt, std::end(featuresStruct));

    /* std::cout << "--------------------------------------------------------------------------------" << std::endl; */
    /* for (const auto& elem :  featuresStruct) */
    /*     std::cout << elem.x << ", " << elem.y << std::endl; */

    // generate id array for later filtering
    std::vector<size_t> uniqueIds;
    uniqueIds.reserve(featuresStruct.size());

    for (const auto& ftStruct : featuresStruct)
        uniqueIds.push_back(ftStruct.idx);

    std::sort(std::begin(uniqueIds), std::end(uniqueIds));
    return uniqueIds;

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
    if (ext > 0)
        extB = true;
    if (up > 0)
        upB = true;

    return cv::xfeatures2d::SURF::create(hess, nOct, nOctL, extB, upB);
}
} // namespace cvutils
