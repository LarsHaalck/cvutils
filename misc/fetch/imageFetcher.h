#ifndef CVUTILS_FETCHER_IMAGE_FETCHER_H
#define CVUTILS_FETCHER_IMAGE_FETCHER_H

#include <filesystem>
#include <fstream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "abstractFetcher.h"

namespace cvutils
{
namespace fetch
{
class ImageFetcher : public AbstractFetcher<size_t, cv::Mat>
{
private:
    std::vector<std::string> mImgFiles;
    std::filesystem::path mImgDir;
    std::filesystem::path mTxtFile;
    cv::ImreadModes mMode;

public:
    ImageFetcher(const std::filesystem::path& imgDir,
        const std::filesystem::path& txtFile, cv::ImreadModes mode)
        : mImgFiles()
        , mImgDir(imgDir)
        , mTxtFile(txtFile)
        , mMode(mode)
    {
        if (!std::filesystem::exists(mImgDir)
            || !std::filesystem::is_directory(mImgDir))
        {
            throw std::filesystem::filesystem_error("Image folder does not exist",
                mImgDir, std::make_error_code(std::errc::no_such_file_or_directory));
        }

        bool hasTxtFile = false;
        if (!mTxtFile.string().empty())
            hasTxtFile = true;

        if (hasTxtFile && (!std::filesystem::exists(mTxtFile)
            || !std::filesystem::is_regular_file(mTxtFile)))
        {
            throw std::filesystem::filesystem_error("Txt file does not exist", mTxtFile,
                std::make_error_code(std::errc::no_such_file_or_directory));
        }

        fillImgFiles();
    }

    size_t size() const
    {
        return mImgFiles.size();
    }

    cv::Mat get(const size_t& idx) const override
    {
        assert("acces out of bounds in getImgFiles()"
            && idx >= 0 && idx < mImgFiles.size());

        return cv::imread(mImgFiles[idx], mMode);
    }

private:
    void fillImgFiles()
    {
        if (mTxtFile.string().empty())
            cv::glob(mImgDir.string(), mImgFiles, false);
        else
        {
            std::ifstream stream(mTxtFile.string());
            std::string currLine;
            while(std::getline(stream, currLine))
            {
                if (!currLine.empty())
                {
                    std::filesystem::path imgPath(currLine);
                    mImgFiles.push_back((mImgDir / imgPath).string());
                }
            }
        }

        std::sort(std::begin(mImgFiles), std::end(mImgFiles));
    }


};
} // namespace fetch
} // namespace cvutils

#endif // CVUTILS_FETCHER_IMAGE_FETCHER_H
