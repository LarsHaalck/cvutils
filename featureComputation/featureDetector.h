#ifndef CVUTILS_FEATURE_DETECTOR_H
#define CVUTILS_FEATURE_DETECTOR_H

#include <filesystem>
#include <string>

#include <opencv2/core.hpp>

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
        std::filesystem::path mInFolder;
        std::filesystem::path mOutFolder;

        bool mHasTxtFile;
        std::filesystem::path mTxtFile;
        std::filesystem::path mFtFile;
    public:
        FeatureDetector(const std::string& inFolder, const std::string& outFolder,
            const std::string& txtFile, const std::string& ftFile);
        void run();
    private:
        std::vector<std::string> getImageFiles();
        cv::Ptr<cv::Feature2D> getFtPtr();
        cv::Ptr<cv::Feature2D> getORBPtr(const cv::FileStorage& fs);
};
}

#endif // CVUTILS_FEATURE_DETECTOR_H
