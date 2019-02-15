#ifndef CVUTILS_MATCHES_WRITER_H
#define CVUTILS_MATCHES_WRITER_H

#define CEREAL_THREAD_SAFE 1
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "io/matchType.h"
#include "io/hash.h"

namespace cereal
{
template<class Archive>
void save(Archive& archive, const cv::DMatch& match)
{
    archive(match.queryIdx, match.trainIdx, match.distance);
}

template<class Archive, typename T, typename S>
void save(Archive& archive, const std::pair<T, S>& pair)
{
    archive(pair.first, pair.second);
}
} // namespace cereal

namespace cvutils
{
class MatchesWriter
{
private:
    std::unordered_map<std::pair<size_t, size_t>, std::vector<cv::DMatch>> mMatches;
    std::filesystem::path mFileName;
    std::ofstream mStream;
public:
    MatchesWriter(const std::filesystem::path& ftDir, MatchType type);
    ~MatchesWriter();


    void writePairWiseMatches(const
        std::unordered_map<std::pair<size_t, size_t>, std::vector<cv::DMatch>>& matches)
    {
        mMatches = matches;
    }
    // use perfect forwarding to insert/emplace into container
    template <typename T>
    void writeMatches(size_t i, size_t j, T&& matches)
    {
        auto pair = std::make_pair(i, j);
        if (!matches.empty() && mMatches.count(pair) == 0)
        {
            mMatches.insert(std::make_pair(pair,
                std::forward<std::vector<cv::DMatch>>(matches)));
        }
    }
};
} // namespace cvutils

#endif // CVUTILS_MATCHES_WRITER_H
