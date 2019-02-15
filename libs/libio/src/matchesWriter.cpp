#include "io/matchesWriter.h"

#include <fstream>

#include "io/config.h"

namespace cvutils
{
MatchesWriter::MatchesWriter(const std::filesystem::path& ftDir, MatchType type)
    : mFileName(ftDir / detail::matchTypeToFileName(type))
    , mStream(mFileName.string(), std::ios::out | std::ios::binary)
{
    if (!std::filesystem::exists(ftDir)
        || !std::filesystem::is_directory(ftDir))
    {
        throw std::filesystem::filesystem_error("Feature folder does not exist",
            ftDir, std::make_error_code(std::errc::no_such_file_or_directory));
    }

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
