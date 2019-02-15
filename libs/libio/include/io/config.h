#ifndef CVUTILS_IO_CONFIG_H
#define CVUTILS_IO_CONFIG_H

#include <string>

#include "io/matchType.h"

namespace cvutils
{
namespace detail
{
constexpr auto ftEnding = "-ft.bin";
constexpr auto ftKey = "pts";

constexpr auto descEnding = "-desc.bin";
constexpr auto descKey = "desc";

constexpr auto matchesPutativeName = "matches-putative.bin";
constexpr auto matchesPutativeFilteredName = "matches-putative-filtered.bin";
constexpr auto matchesGeometricName = "matches-geometric.bin";
constexpr auto matchesGeometricFilteredName = "matches-geometric-filtered.bin";
constexpr auto pairMatKey = "pairMat";

const std::string idToMatchKey(size_t i, size_t j);
const std::string matchTypeToFileName(cvutils::MatchType type);

} // namespace detail
} // namespace cvutils
#endif // CVUTILS_IO_CONFIG_H
