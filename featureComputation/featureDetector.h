#ifndef CVUTILS_FEATURE_DETECTOR_H
#define CVUTILS_FEATURE_DETECTOR_H

#include <string>

#include <opencv2/core.hpp>
//#include <opencv2/features2d.hpp>
//#include <opencv2/xfeatures2d.hpp>

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
        std::string mInFolder;
        std::string mOutFolder;
        std::string mFtFile;
    public:
        FeatureDetector(const std::string& inFolder,
            const std::string& outFolder, const std::string& ftFile);
        void run();
    private:
        cv::Ptr<cv::Feature2D> getFtPtr();
        cv::Ptr<cv::Feature2D> getORBPtr(const cv::FileStorage& fs);
};
}

#endif // CVUTILS_FEATURE_DETECTOR_H
