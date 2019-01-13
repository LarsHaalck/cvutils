#include "io.h"

#include <algorithm>
#include <fstream>

namespace cvutils
{
namespace misc
{

std::vector<std::string> getImgFiles(const std::filesystem::path& imgDir,
        const std::filesystem::path& txtFile)
{
    std::vector<std::string> files;
    if (txtFile.string().empty())
        cv::glob(imgDir.string(), files, false);
    else
    {
        std::ifstream stream(txtFile.string());
        std::string currLine;
        while(std::getline(stream, currLine))
        {
            if (!currLine.empty())
            {
                std::filesystem::path imgPath(currLine);
                files.push_back((imgDir / imgPath).string());
            }
        }
    }

    std::sort(std::begin(files), std::end(files));
    return files;
}

std::vector<std::vector<cv::KeyPoint>> getFtVecs(
    const std::vector<std::string>& imgFiles, const std::filesystem::path& ftDir)
{
    std::vector<std::vector<cv::KeyPoint>> keyPoints;
    for (const auto& file : imgFiles)
    {
        std::filesystem::path imgStem(file);
        imgStem = ftDir / imgStem.stem();

        cv::FileStorage fsFt(imgStem.string() + "-feat.yml", cv::FileStorage::READ);
        std::vector<cv::KeyPoint> currKeyPts;
        cv::read(fsFt["pts"], currKeyPts);
        keyPoints.push_back(currKeyPts);
    }

    return keyPoints;
}
}
}
