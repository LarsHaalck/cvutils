#ifndef CVUTILS_IO_CONFIG_H
#define CVUTILS_IO_CONFIG_H

#include <string>

#include "io/matchType.h"

namespace cvutils
{
namespace detail
{
constexpr auto ftEnding = "-ft.yml.gz";
constexpr auto ftKey = "pts";

constexpr auto descEnding = "-desc.yml.gz";
constexpr auto descKey = "desc";

constexpr auto matchesPutativeName = "matches-putative.yml.gz";
constexpr auto matchesPutativeFilteredName = "matches-putative-filtered.yml.gz";
constexpr auto matchesGeometricName = "matches-geometric.yml.gz";
constexpr auto matchesGeometricFilteredName = "matches-geometric-filtered.yml.gz";
constexpr auto pairMatKey = "pairMat";

const std::string idToMatchKey(size_t i, size_t j);
const std::string matchTypeToFileName(cvutils::MatchType type);

} // namespace detail
} // namespace cvutils
#endif // CVUTILS_IO_CONFIG_H
