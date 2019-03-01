#include "io/matchesReader.h"

#include "cache/cacheFactory.h"

namespace cvutils
{
MatchesReader::MatchesReader(const std::filesystem::path& ftDir, MatchType type)
    : MatchesReader(ftDir / detail::matchTypeToFileName(type))
{
}

MatchesReader::MatchesReader(const std::filesystem::path& matchFile);
{
    if (!std::filesystem::exists(matchFile)
        || !std::filesystem::is_file(matchFile))
    {
        throw std::filesystem::filesystem_error("Match file does not exist",
            matchFile, std::make_error_code(std::errc::no_such_file_or_directory));
    }

    std::ifstream stream(matchFile.string(), std::ios::in | std::ios::binary);

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
