#include "io/matchesReader.h"

#include "cache/cacheFactory.h"

namespace cvutils
{
MatchesReader::MatchesReader(const std::filesystem::path& ftDir, MatchType type)
{
    if (!std::filesystem::exists(ftDir)
        || !std::filesystem::is_directory(ftDir))
    {
        throw std::filesystem::filesystem_error("Feature folder does not exist",
            ftDir, std::make_error_code(std::errc::no_such_file_or_directory));
    }

    auto fileName = (ftDir / detail::matchTypeToFileName(type));
    std::ifstream stream(fileName.string(), std::ios::in | std::ios::binary);

    if (!stream.is_open())
    {
        throw std::filesystem::filesystem_error("Error opening matches file",
            fileName, std::make_error_code(std::errc::io_error));
    }

    {
        cereal::PortableBinaryInputArchive archive(stream);
        archive(mMatches);
        stream.close();
    }
}

size_t MatchesReader::numMatches() const { return mMatches.size(); }

std::vector<cv::DMatch> MatchesReader::getMatches(size_t idI, size_t idJ) const
{
    return mMatches.at(std::make_pair(idI, idJ));
}

std::unordered_map<std::pair<size_t, size_t>, std::vector<cv::DMatch>>
MatchesReader::moveMatches()
{
    return std::move(mMatches);
}

std::vector<std::pair<size_t, size_t>> MatchesReader::getMatchPairs() const
{
    std::vector<std::pair<size_t, size_t>> keys;
    keys.reserve(mMatches.size());

    for(const auto& matches : mMatches)
        keys.push_back(matches.first);

    return keys;
}

} // namespace cvutils
