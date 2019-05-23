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
    bool mRemoveDuplicates;

    struct FeatureStruct
    {
        float x, y;
        size_t idx;

        FeatureStruct(const cv::KeyPoint& aFeature, size_t aIdx)
            : x(aFeature.pt.x)
            , y(aFeature.pt.y)
            , idx(aIdx)
        {
        }

        bool operator<(const FeatureStruct& rhs) const;
        bool operator==(const FeatureStruct& rhs) const;
    };

public:
    FeatureDetector(const std::string& imgDir, const std::string& txtFile,
            const std::string& ftDir, const std::string& ftFile, float scale,
            bool removeDuplicates);
    void run();
private:
    cv::Ptr<cv::Feature2D> getFtPtr();
    cv::Ptr<cv::Feature2D> getORBPtr(const cv::FileStorage& fs);
    cv::Ptr<cv::Feature2D> getSIFTPtr(const cv::FileStorage& fs);
    cv::Ptr<cv::Feature2D> getSURFPtr(const cv::FileStorage& fs);
    std::vector<size_t> getUniqueIds(const std::vector<cv::KeyPoint>& features);
};
}

#endif // CVUTILS_FEATURE_DETECTOR_H
