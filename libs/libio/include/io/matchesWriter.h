#ifndef CVUTILS_MATCHES_WRITER_H
#define CVUTILS_MATCHES_WRITER_H

#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "io/matchType.h"

namespace cvutils
{
class MatchesWriter
{
private:
    cv::FileStorage mFile;
public:
    MatchesWriter(const std::filesystem::path& ftDir, MatchType type);

    std::pair<size_t, std::vector<bool>> getPairMatMask(
        const std::vector<size_t>& matchSizes);

    void writePairMat(const cv::Mat& pairMat);
    void writePairMat(const cv::Mat& pairMat, const std::vector<size_t> matchSizes);
    void writeMatches(size_t i, size_t j, const std::vector<cv::DMatch>& matches);
};
} // namespace cvutils

#endif // CVUTILS_MATCHES_WRITER_H
