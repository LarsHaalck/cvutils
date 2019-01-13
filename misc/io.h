#ifndef CVUTILS_MISC_IO_H
#define CVUTILS_MISC_IO_H

#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/core.hpp>

namespace cvutils
{
namespace misc
{
std::vector<std::string> getImgFiles(const std::filesystem::path& imgDir,
    const std::filesystem::path& txtFile);

std::vector<std::vector<cv::KeyPoint>> getFtVecs(
    const std::vector<std::string>& imgFiles, const std::filesystem::path& ftDir);
}
}


#endif // CVUTILS_MISC_IO_H
