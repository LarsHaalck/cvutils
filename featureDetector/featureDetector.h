#ifndef CVUTILS_FEATURE_DETECTOR_H
#define CVUTILS_FEATURE_DETECTOR_H

#include <filesystem>
#include <string>

#include <opencv2/core.hpp>

#include "io/imageReader.h"
#include "io/featureWriter.h"

// forward declarations
namespace cv
{
    class Feature2D;
}

namespace cvutils
{
class FeatureDetector
{
    private:
        cvutils::io::ImageReader mReader;
        cvutils::io::FeatureWriter mWriter;
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
