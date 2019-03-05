#include "io/matchesWriter.h"

#include <fstream>

#include "io/config.h"

namespace cvutils
{
MatchesWriter::MatchesWriter(const std::filesystem::path& ftDir, GeometricType type)
    : MatchesWriter(ftDir/ detail::geometricTypeToFileName(type))
{
}

MatchesWriter::MatchesWriter(const std::filesystem::path& matchFile)
    : mFileName(matchFile)
    , mStream(matchFile.string(), std::ios::out | std::ios::binary)
{
    if (!mStream.is_open())
    {
        throw std::filesystem::filesystem_error("Error opening matches file",
            mFileName, std::make_error_code(std::errc::io_error));
    }
}
MatchesWriter::~MatchesWriter()
{
    {
        cereal::PortableBinaryOutputArchive archive(mStream);
        archive(mMatches);
        mStream.close();
    }
}
} // namespace cvutils
