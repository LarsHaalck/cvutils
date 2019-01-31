#include "io.h"

#include <algorithm>
#include <fstream>

#include "tqdm.h"

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
    std::vector<std::vector<cv::KeyPoint>> keyPoints(imgFiles.size());

    std::cout << "Loading feature files" << std::endl;
    tqdm bar;
    size_t count = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < imgFiles.size(); i++)
    {
        std::string file = imgFiles[i];
        std::filesystem::path imgStem(file);
        imgStem = ftDir / imgStem.stem();

        cv::FileStorage fsFt(imgStem.string() + "-feat.yml.gz", cv::FileStorage::READ);
        std::vector<cv::KeyPoint> currKeyPts;
        cv::read(fsFt["pts"], currKeyPts);
        keyPoints[i] = currKeyPts;

        #pragma omp critical
        bar.progress(count++, imgFiles.size());

    }
    return keyPoints;
}


std::vector<cv::Mat> getDescMats(
    const std::vector<std::string>& imgFiles, const std::filesystem::path& ftDir)
{
    std::vector<cv::Mat> desc(imgFiles.size());

    std::cout << "Loading descriptor files" << std::endl;
    tqdm bar;
    size_t count = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < imgFiles.size(); i++)
    {
        std::string file = imgFiles[i];
        std::filesystem::path imgStem(file);
        imgStem = ftDir / imgStem.stem();

        cv::FileStorage fsFt(imgStem.string() + "-desc.yml.gz", cv::FileStorage::READ);
        cv::Mat currDesc;
        cv::read(fsFt["desc"], currDesc);
        desc[i] = currDesc;

        #pragma omp critical
        bar.progress(count++, imgFiles.size());
    }

    return desc;
}


std::pair<cv::Mat, std::vector<std::vector<cv::DMatch>>> getMatches(
    const std::filesystem::path& ftDir,
    detail::MatchType type)
{
    std::cout << "Loading matches file" << std::endl;
    std::string ending = (type == detail::MatchType::Putative)
        ? "-putative.yml.gz"
        : "-geometric.yml.gz";

    std::filesystem::path base("matches");
    base = ftDir / base;
    cv::FileStorage matchFile(base.string() + ending, cv::FileStorage::READ);
    cv::Mat pairMat;
    cv::read(matchFile["pairMat"], pairMat);

    std::vector<std::vector<cv::DMatch>> matches;
    for (int i = 0; i < pairMat.rows; i++)
    {
        std::vector<cv::DMatch> currMatches;
        cv::read(matchFile[std::string("matches_") + std::to_string(i)], currMatches);
        matches.push_back(currMatches);
    }

    return {pairMat, matches};

}

}
}
