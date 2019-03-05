#ifndef CVUTILS_MATCHES_READER_H
#define CVUTILS_MATCHES_READER_H

#define CEREAL_THREAD_SAFE 1
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

#include <opencv2/core.hpp>

#include "cache/abstractCache.h"
#include "io/geometricType.h"

namespace cereal
{
template<class Archive>
void load(Archive& archive, cv::DMatch& match)
{
    archive(match.queryIdx, match.trainIdx, match.distance);
}

template<class Archive, typename T, typename S>
void load(Archive& archive, std::pair<T, S>& pair)
{
    archive(pair.first, pair.second);
}
} // namespace cereal

namespace cvutils
{
class MatchesReader
{
private:
    std::unordered_map<std::pair<size_t, size_t>, std::vector<cv::DMatch>> mMatches;
public:
    MatchesReader(const std::filesystem::path& ftDir, GeometricType type);
    MatchesReader(const std::filesystem::path& ftDir);

    size_t numMatches() const;
    std::vector<cv::DMatch> getMatches(size_t idI, size_t idJ) const;

    std::unordered_map<std::pair<size_t, size_t>, std::vector<cv::DMatch>>
    moveMatches();

    std::vector<std::pair<size_t, size_t>> getMatchPairs() const;
};
} // namespace cvutils

#endif // CVUTILS_MATCHES_READER_H
