#include "io/config.h"

namespace cvutils
{
namespace detail
{

const std::string geometricTypeToFileName(cvutils::GeometricType type)
{
    switch (type)
    {
        case cvutils::GeometricType::Putative:
            return matchesPutativeName;
        case cvutils::GeometricType::Isometry:
            return matchesIsometryName;
        case cvutils::GeometricType::Similarity:
            return matchesSimilarityName;
        case cvutils::GeometricType::Affine:
            return matchesAffineName;
        case cvutils::GeometricType::Homography:
            return matchesHomographyName;
    }
    return "";
}

} // namespace detail
} // namespace cvutils
