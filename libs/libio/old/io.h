#ifndef CVUTILS_MISC_IO_H
#define CVUTILS_MISC_IO_H

#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/core.hpp>

namespace cvutils
{
namespace detail
{
    enum class MatchType
    {
        Putative,
        PutativeFiltered,
        Geometric,
        GeometricFiltered
    };
}
namespace misc
{
enum class IOType
{
    Read,
    Write
};

class IO
{
////////////////////////////////////////
//         NON-STATIC STUFF           //
////////////////////////////////////////
public:
    IO(const std::filesystem::path& ftDir, detail::MatchType matchType, IOType readType);
    void writeMatch(size_t i, size_t j, const std::vector<cv::DMatch>& match);
    std::vector<cv::DMatch> readMatch(size_t i, size_t j);

    void writePairMat(const cv::Mat& pairMat, const std::vector<size_t>& matchSizes);
    cv::Mat readPairMat();
private:
    std::filesystem::path mFtDir;
    detail::MatchType mType;
    cv::FileStorage mFile;

////////////////////////////////////////
//          STATIC STUFF              //
////////////////////////////////////////
public:
    static std::vector<std::string> getImgFiles(const std::filesystem::path& imgDir,
        const std::filesystem::path& txtFile);
    // Feature vecs
    static std::vector<std::vector<cv::KeyPoint>> getFtVecs(
        const std::vector<std::string>& imgFiles, const std::filesystem::path& ftDir);
    static std::vector<cv::KeyPoint> getFtVec(const std::string& imgFile,
        const std::filesystem::path& ftDir);

    // desc mats
    static std::vector<cv::Mat> getDescMats(const std::vector<std::string>& imgFiles,
            const std::filesystem::path& ftDir);
    static cv::Mat getDescMat(const std::string& imgFile, const std::filesystem::path& ftDir);

    // matches
    static std::pair<cv::Mat, std::vector<std::vector<cv::DMatch>>> getMatches(
        const std::filesystem::path& ftDir, detail::MatchType type);

    static void writeMatches(const std::filesystem::path& ftDir, const cv::Mat& pairMat,
        const std::vector<std::vector<cv::DMatch>>& matches, detail::MatchType type);


    static std::string getMatchesFilename(const std::filesystem::path& ftDir,
        detail::MatchType type);
    static std::string typeToFileEnding(detail::MatchType type);

private:
    static std::pair<size_t, std::vector<bool>> getPairMatMask(
        const std::vector<std::vector<cv::DMatch>>& matches);
    static std::pair<size_t, std::vector<bool>> getPairMatMask(
        const std::vector<size_t>& matchSizes);


};

}
}


#endif // CVUTILS_MISC_IO_H
