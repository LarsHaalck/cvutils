#include "io/matchesWriter.h"

#include "io/config.h"

namespace cvutils
{
MatchesWriter::MatchesWriter(const std::filesystem::path& ftDir, MatchType type)
{
    if (!std::filesystem::exists(ftDir)
        || !std::filesystem::is_directory(ftDir))
    {
        throw std::filesystem::filesystem_error("Feature folder does not exist",
            ftDir, std::make_error_code(std::errc::no_such_file_or_directory));
    }

    // adds filename to ftDir (implicitly casted to filesystem::path)
    auto fileName = (ftDir / detail::matchTypeToFileName(type));
    mFile = cv::FileStorage(fileName.string(), cv::FileStorage::WRITE);
}


std::pair<size_t, std::vector<bool>> MatchesWriter::getPairMatMask(
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

void MatchesWriter::writePairMat(const cv::Mat& pairMat)
{
    cv::write(mFile, detail::pairMatKey, pairMat);
}

void MatchesWriter::writePairMat(const cv::Mat& pairMat, const std::vector<size_t> matchSizes)
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

    writePairMat(truncPairMat);
}

void MatchesWriter::writeMatches(size_t i, size_t j,
    const std::vector<cv::DMatch>& matches)
{
    // when reading, the default is empty, so we don't need to store this
    if (!matches.empty())
    {
        const auto key = detail::idToMatchKey(i, j);
        cv::write(mFile, key, matches);
    }
}
} // namespace cvutils
