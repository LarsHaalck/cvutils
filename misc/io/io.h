#ifndef CVUTILS_MISC_IO_H
#define CVUTILS_MISC_IO_H

#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/core.hpp>

namespace cvutils
{
namespace detail
{
    enum class MatchType
    {
        Putative,
        PutativeFiltered,
        Geometric,
        GeometricFiltered
    };
}
namespace misc
{
std::vector<std::string> getImgFiles(const std::filesystem::path& imgDir,
    const std::filesystem::path& txtFile);

std::vector<std::vector<cv::KeyPoint>> getFtVecs(
    const std::vector<std::string>& imgFiles, const std::filesystem::path& ftDir);
std::vector<cv::Mat> getDescMats(
    const std::vector<std::string>& imgFiles, const std::filesystem::path& ftDir);

std::pair<cv::Mat, std::vector<std::vector<cv::DMatch>>> getMatches(
    const std::filesystem::path& ftDir, detail::MatchType type);

std::pair<size_t, std::vector<bool>> getPairMatMask(
    const std::vector<std::vector<cv::DMatch>>& matches);

void writeMatches(const std::filesystem::path& ftDir, const cv::Mat& pairMat,
    const std::vector<std::vector<cv::DMatch>>& matches, detail::MatchType type);

std::string typeToFiledEnding(detail::MatchType type);

}
}


#endif // CVUTILS_MISC_IO_H
