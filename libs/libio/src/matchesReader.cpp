#include "io/matchesReader.h"

#include "cache/cacheFactory.h"

using idPair = std::pair<size_t, size_t>;
using Matches = std::vector<cv::DMatch>;

namespace cvutils
{
MatchesReader::MatchesReader(const std::filesystem::path& ftDir, MatchType type,
    int cacheSize)
    : mFetcher(std::make_shared<detail::MatchesFetcher>(ftDir, type))
    , mSize(mFetcher->size())
    , mCache(detail::createCachePtr<idPair, Matches>(mFetcher, cacheSize))
{

}

size_t MatchesReader::numMatches() const { return mSize; }

std::vector<cv::DMatch> MatchesReader::getMatches(size_t idI, size_t idJ)
{
    return mCache->get(std::make_pair(idI, idJ));
}

cv::Mat MatchesReader::getPairMat() const
{
   return mFetcher->getPairMat();
}
} // namespace cvutils
