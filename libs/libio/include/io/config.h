#ifndef CVUTILS_IO_CONFIG_H
#define CVUTILS_IO_CONFIG_H

#include <string>

#include "io/GeometricType.h"

namespace cvutils
{
namespace detail
{
constexpr auto ftEnding = "-ft.bin";
constexpr auto ftKey = "pts";

constexpr auto descEnding = "-desc.bin";
constexpr auto descKey = "desc";

constexpr auto matchesPutativeName = "matches-putative.bin";
constexpr auto matchesIsometryName = "matches-isomery.bin";
constexpr auto matchesSimilarityName = "matches-similarity.bin";
constexpr auto matchesAffineName = "matches-affine.bin";
constexpr auto matchesHomographyName = "matches-homography.bin";

const std::string geometricTypeToFileName(cvutils::GeometricType type);

} // namespace detail
} // namespace cvutils
#endif // CVUTILS_IO_CONFIG_H
