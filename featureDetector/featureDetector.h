#ifndef CVUTILS_FEATURE_DETECTOR_H
#define CVUTILS_FEATURE_DETECTOR_H

#include <filesystem>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "io/imageReader.h"
#include "io/featureWriter.h"
#include "io/descriptorWriter.h"


namespace cvutils
{
class FeatureDetector
{
private:
    ImageReader mReader;
    FeatureWriter mFtWriter;
    DescriptorWriter mDescWriter;
    std::filesystem::path mFtFile;

public:
    FeatureDetector(const std::string& inFolder, const std::string& outFolder,
        const std::string& txtFile, const std::string& ftFile, float scale);
    void run();
private:
    cv::Ptr<cv::Feature2D> getFtPtr();
    cv::Ptr<cv::Feature2D> getORBPtr(const cv::FileStorage& fs);
    cv::Ptr<cv::Feature2D> getSIFTPtr(const cv::FileStorage& fs);
    cv::Ptr<cv::Feature2D> getSURFPtr(const cv::FileStorage& fs);
};
}

#endif // CVUTILS_FEATURE_DETECTOR_H
