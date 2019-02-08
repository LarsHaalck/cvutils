#include "io/config.h"

namespace cvutils
{
namespace detail
{

const std::string idToMatchKey(size_t i, size_t j)
{
    return std::string("match_") + std::to_string(i) + "_" + std::to_string(j);
}

const std::string matchTypeToFileName(cvutils::MatchType type)
{
    switch (type)
    {
        case cvutils::MatchType::Putative:
            return matchesPutativeName;
        case cvutils::MatchType::PutativeFiltered:
            return matchesPutativeFilteredName;
        case cvutils::MatchType::Geometric:
            return matchesGeometricName;
        case cvutils::MatchType::GeometricFiltered:
            return matchesGeometricFilteredName;
    }
    return "";
}

} // namespace detail
} // namespace cvutils
