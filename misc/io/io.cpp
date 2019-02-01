#include "io.h"

#include <algorithm>
#include <fstream>

#include "tqdm.h"

namespace cvutils
{
namespace misc
{

////////////////////////////////////////
//         NON-STATIC STUFF           //
////////////////////////////////////////
IO::IO(const std::filesystem::path& ftDir, detail::MatchType matchType, IOType readType)
    : mFtDir(ftDir)
    , mType(matchType)
{
    auto fileName = getMatchesFilename(ftDir, matchType);
    cv::FileStorage::Mode mode = (readType == IOType::Read)
        ? cv::FileStorage::READ
        : cv::FileStorage::WRITE;
    mFile = cv::FileStorage(fileName, mode);

    if (!mFile.isOpened())
    {
        throw std::filesystem::filesystem_error("Error opening matches file",
            fileName, std::make_error_code(std::errc::io_error));
    }
}
void IO::writeMatch(size_t i, size_t j, const std::vector<cv::DMatch>& match)
{
    // when reading, the default is empty, so we don't need to store this
    if (!match.empty())
    {
        cv::write(mFile, std::string("matches_") + std::to_string(i) + "_"
            + std::to_string(j), match);
    }
}

std::vector<cv::DMatch> IO::readMatch(size_t i, size_t j)
{
    std::vector<cv::DMatch> currMatches;
    auto accessString = "matches_" + std::to_string(i) + "_" + std::to_string(j);
    cv::read(mFile[accessString], currMatches);
    return currMatches;
}

void IO::writePairMat(const cv::Mat& pairMat, const std::vector<size_t>& matchSizes)
{
    auto sizeIdPair = getPairMatMask(matchSizes);
    auto truncPairMat = cv::Mat(sizeIdPair.first, 2, pairMat.type());
    for(int r = 0, k = 0; r < pairMat.rows; r++)
    {
        if (sizeIdPair.second[r])
        {
            truncPairMat.at<int>(k, 0) = pairMat.at<int>(r, 0);
            truncPairMat.at<int>(k++, 1) = pairMat.at<int>(r, 1);
        }
    }

    std::cout << "Writing pair mat" << std::endl;
    cv::write(mFile, "pairMat", truncPairMat);

}

cv::Mat IO::readPairMat()
{
    cv::Mat pairMat;
    cv::read(mFile["pairMat"], pairMat);
    return pairMat;
}

////////////////////////////////////////
//          STATIC STUFF              //
////////////////////////////////////////
std::vector<std::string> IO::getImgFiles(const std::filesystem::path& imgDir,
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

std::vector<std::vector<cv::KeyPoint>> IO::getFtVecs(
    const std::vector<std::string>& imgFiles, const std::filesystem::path& ftDir)
{
    std::vector<std::vector<cv::KeyPoint>> keyPoints(imgFiles.size());

    std::cout << "\nLoading feature files" << std::endl;
    tqdm bar;
    size_t count = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < imgFiles.size(); i++)
    {
        keyPoints[i] = getFtVec(imgFiles[i], ftDir);

        #pragma omp critical
        bar.progress(count++, imgFiles.size());

    }
    return keyPoints;
}

std::vector<cv::KeyPoint> IO::getFtVec(const std::string& imgFile,
    const std::filesystem::path& ftDir)
{
    std::filesystem::path imgStem(imgFile);
    imgStem = ftDir / imgStem.stem();

    cv::FileStorage fsFt(imgStem.string() + "-feat.yml.gz", cv::FileStorage::READ);
    std::vector<cv::KeyPoint> currKeyPts;
    cv::read(fsFt["pts"], currKeyPts);

    return currKeyPts;
}

std::vector<cv::Mat> IO::getDescMats(
    const std::vector<std::string>& imgFiles, const std::filesystem::path& ftDir)
{
    std::vector<cv::Mat> desc(imgFiles.size());

    std::cout << "\nLoading descriptor files" << std::endl;
    tqdm bar;
    size_t count = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < imgFiles.size(); i++)
    {
        desc[i] = getDescMat(imgFiles[i], ftDir);

        #pragma omp critical
        bar.progress(count++, imgFiles.size());
    }

    return desc;
}

cv::Mat IO::getDescMat(const std::string& imgFile, const std::filesystem::path& ftDir)
{
    std::filesystem::path imgStem(imgFile);
    imgStem = ftDir / imgStem.stem();

    cv::FileStorage fsFt(imgStem.string() + "-desc.yml.gz", cv::FileStorage::READ);
    cv::Mat currDesc;
    cv::read(fsFt["desc"], currDesc);
    return currDesc;
}


std::string IO::getMatchesFilename(const std::filesystem::path& ftDir,
    detail::MatchType type)
{
    std::filesystem::path base("matches");
    base = ftDir / base;
    return base.string() + typeToFileEnding(type);
}

std::string IO::typeToFileEnding(detail::MatchType type)
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

std::pair<cv::Mat, std::vector<std::vector<cv::DMatch>>> IO::getMatches(
    const std::filesystem::path& ftDir,
    detail::MatchType type)
{
    std::cout << "Loading matches file" << std::endl;
    auto fileName = getMatchesFilename(ftDir, type);

    cv::FileStorage matchFile(fileName, cv::FileStorage::READ);
    if (!matchFile.isOpened())
    {
        std::cout << "Could not read file " << fileName << std::endl;
        std::cout << "Does it exist?" << std::endl;
        return {};
    }
    cv::Mat pairMat;
    cv::read(matchFile["pairMat"], pairMat);

    std::vector<std::vector<cv::DMatch>> matches;
    for (int r = 0; r < pairMat.rows; r++)
    {
        size_t i = pairMat.at<int>(r, 0);
        size_t j = pairMat.at<int>(r, 1);

        auto accesString = "matches_" + std::to_string(i) + std::to_string(j);
        std::vector<cv::DMatch> currMatches;
        cv::read(matchFile[accesString], currMatches);
        matches.push_back(currMatches);
    }

    std::cout << "Read file " << fileName << std::endl;
    std::cout << "with #" << pairMat.rows << " correspondences" << std::endl;

    return {pairMat, matches};

}

std::pair<size_t, std::vector<bool>> IO::getPairMatMask(
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

std::pair<size_t, std::vector<bool>> IO::getPairMatMask(
    const std::vector<size_t>& matchSizes)
{
    auto ids = std::vector<bool>(matchSizes.size(), false);
    size_t count = 0;
    for (size_t i = 0; i < matchSizes.size(); i++)
    {
        if (!matchSizes[i])
        {
            ids[i] = true;
            count++;
        }
    }
    return {count, ids};
}

void IO::writeMatches(const std::filesystem::path& ftDir, const cv::Mat& pairMat,
    const std::vector<std::vector<cv::DMatch>>& matches, detail::MatchType type)
{
    auto fileName = getMatchesFilename(ftDir, type);
    cv::FileStorage matchFile(fileName, cv::FileStorage::WRITE);

    if (!matchFile.isOpened())
    {
        std::cout << "Could not open matches file for writing" << std::endl;
        return;
    }

    std::cout << "Writing to " << fileName << std::endl;

    auto sizeIdPair = getPairMatMask(matches);
    auto truncPairMat = cv::Mat(sizeIdPair.first, 2, pairMat.type());
    for(int r = 0, k = 0; r < pairMat.rows; r++)
    {
        if (sizeIdPair.second[r])
        {
            truncPairMat.at<int>(k, 0) = pairMat.at<int>(r, 0);
            truncPairMat.at<int>(k++, 1) = pairMat.at<int>(r, 1);

            size_t i = pairMat.at<int>(r, 0);
            size_t j = pairMat.at<int>(r, 1);
            auto accesString = "matches_" + std::to_string(i) + std::to_string(j);
            cv::write(matchFile, accesString, matches[r]);
        }
    }
    cv::write(matchFile, "pairMat", truncPairMat);
}

}
}
