#ifndef CVUTILS_IO_FEATURE_WRITE_H
#define CVUTILS_IO_FEATURE_WRITE_H

#define CEREAL_THREAD_SAFE 1
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>

#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/features2d.hpp>

namespace cereal
{
template<class Archive>
void save(Archive& archive, const cv::KeyPoint& kp)
{
    archive(kp.pt.x, kp.pt.y, kp.size, kp.angle, kp.response, kp.octave, kp.class_id);
}
} // namespace cereal


namespace cvutils
{
class FeatureWriter
{
private:
    std::filesystem::path mFtDir;
public:
    FeatureWriter(const std::filesystem::path& ftDir);
    void writeFeatures(const std::string& imgFilePath,
        const std::vector<cv::KeyPoint>& features);
};
} // namespace cvutils

#endif // CVUTILS_IO_FEATURE_WRITE_H
