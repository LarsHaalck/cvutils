#ifndef CVUTILS_IO_MATCH_TYPE_H
#define CVUTILS_IO_MATCH_TYPE_H

namespace cvutils
{
enum class MatchType
{
    Putative,
    Isometry,
    Similarity,
    Affine,
    Homography,
};
} // namespace cvutils
#endif // CVUTILS_IO_MATCH_TYPE_H

