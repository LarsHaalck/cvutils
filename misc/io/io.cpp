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

    std::cout << "\nLoading feature files" << std::endl;
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

    std::cout << "\nLoading descriptor files" << std::endl;
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


std::string typeToFiledEnding(detail::MatchType type)
{
    std::string ending;
    switch (type)
    {
        case detail::MatchType::Putative:
            ending = "-putative.yml.gz";
            break;
        case detail::MatchType::PutativeFiltered:
            ending = "-putative-filtered.yml.gz";
            break;
        case detail::MatchType::Geometric:
            ending = "-geometric.yml.gz";
            break;
        case detail::MatchType::GeometricFiltered:
            ending = "-geometric-filtered.yml.gz";
            break;
    }
    return ending;
}

std::pair<cv::Mat, std::vector<std::vector<cv::DMatch>>> getMatches(
    const std::filesystem::path& ftDir,
    detail::MatchType type)
{
    std::cout << "Loading matches file" << std::endl;
    auto ending = typeToFiledEnding(type);

    std::filesystem::path base("matches");
    base = ftDir / base;
    cv::FileStorage matchFile(base.string() + ending, cv::FileStorage::READ);
    if (!matchFile.isOpened())
    {
        std::cout << "Could not read file " << base.string() + ending << std::endl;
        std::cout << "Does ist exist?" << std::endl;
        return {};
    }
    cv::Mat pairMat;
    cv::read(matchFile["pairMat"], pairMat);

    std::vector<std::vector<cv::DMatch>> matches;
    for (int i = 0; i < pairMat.rows; i++)
    {
        std::vector<cv::DMatch> currMatches;
        cv::read(matchFile[std::string("matches_") + std::to_string(i)], currMatches);
        matches.push_back(currMatches);
    }

    std::cout << "Read file " << base.string()  + ending << std::endl;
    std::cout << "with #" << pairMat.rows << " correspondences" << std::endl;

    return {pairMat, matches};

}

std::pair<size_t, std::vector<bool>> getPairMatMask(
    const std::vector<std::vector<cv::DMatch>>& matches)
{
    auto ids = std::vector<bool>(matches.size(), false);
    size_t count = 0;
    for (size_t i = 0; i < matches.size(); i++)
    {
        if (!matches[i].empty())
        {
            ids[i] = true;
            count++;
        }
    }
    return {count, ids};
}

void writeMatches(const std::filesystem::path& ftDir, const cv::Mat& pairMat,
    const std::vector<std::vector<cv::DMatch>>& matches, detail::MatchType type)
{
    auto ending = typeToFiledEnding(type);
    std::filesystem::path base("matches");
    base = ftDir / base;
    cv::FileStorage matchFile(base.string() + ending,
        cv::FileStorage::WRITE);

    if (!matchFile.isOpened())
    {
        std::cout << "Could not open matches file for writing" << std::endl;
        return;
    }
    
    auto sizeIdPair = getPairMatMask(matches);
    auto truncPairMat = cv::Mat(sizeIdPair.first, 2, pairMat.type());
    for(int r = 0, k = 0; r < pairMat.rows; r++)
    {
        if (sizeIdPair.second[r])
        {
            truncPairMat.at<int>(k, 0) = pairMat.at<int>(r, 0);
            truncPairMat.at<int>(k++, 1) = pairMat.at<int>(r, 1);
        }
    }

    std::cout << "Writing to " << base.string() + ending << std::endl;
    cv::write(matchFile, "pairMat", truncPairMat);
    size_t i = 0;
    for (const auto& match : matches)
    {
        if (!match.empty())
            cv::write(matchFile, std::string("matches_") + std::to_string(i++), match);
    }

}

}
}
