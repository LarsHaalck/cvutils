#include <iostream>
#include <string>

#include "cxxopts.hpp"
#include "featureMatcher.h"
#include "io/geometricType.h"

#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
    std::string inFolder, txtFile;
    std::string ftFolder;
    bool isBinary;
    int matcher, window;
    int cacheSize;
    std::vector<char> geoms;
    bool prune;
    bool checkSymmetry;
    double condition;
    double minDist;
    double minCoverage;
    std::string camFile;

    cxxopts::Options options("ftMatch", "Feature matcher + export helper");
    options.add_options()
        ("i,in", "image folder", cxxopts::value(inFolder))
        ("t,txt", "txt file", cxxopts::value(txtFile))
        ("f,featureFolder", "feature folder", cxxopts::value(ftFolder))
        ("b,binaryFeature", "set if used feature was binary (e.g. ORB)",
            cxxopts::value(isBinary))
        ("m,matcher", "matcher method (0, 1) for (brute, flann), default 0",
            cxxopts::value(matcher))
        ("g,geometric", "geometric model for filtering (can be repeated): \
            i (isometric), s (similarity), a (affine), h (homography) default h",
            cxxopts::value(geoms))
        ("w,window", "window size (use all pairs if skipped)", cxxopts::value(window))
        ("p,prune", "prune non-consecutive matches", cxxopts::value(prune))
        ("k,condition", "condition number filtering", cxxopts::value(condition))
        ("d,distance", "min distance for flann based", cxxopts::value(minDist))
        ("o,coverage", "min coverage of match bounding box", cxxopts::value(minCoverage))
        ("s,symmetry", "check symmetriy of matching", cxxopts::value(checkSymmetry))
        ("c,cache", "cache size (if loading all into RAM is not feasable) (< 0) means \
            inf cache, (= 0) means no cache", cxxopts::value(cacheSize))
        ("l,lens", "lens distortion and camera matrix file", cxxopts::value(camFile));

    auto result = options.parse(argc, argv);
    if (result.count("in") != 1
        || result.count("featureFolder") != 1)
    {
        std::cout << options.help() << std::endl;
        return -1;
    }

    if (result.count("binaryFeature") != 1)
            isBinary = false;

    if (result.count("matcher") != 1 || matcher < 0 || matcher > 1)
    {
        std::cout << "Using brute force because matcher omitted or invalid" << std::endl;
        matcher = 0;
    }

    cvutils::GeometricType types = cvutils::GeometricType::Undefined;
    for(auto g : geoms)
    {
        switch(g)
        {
            case 'i':
                types |= cvutils::GeometricType::Isometry;
                break;
            case 's':
                types |= cvutils::GeometricType::Similarity;
                break;
            case 'a':
                types |= cvutils::GeometricType::Affinity;
                break;
            case 'h':
                types |= cvutils::GeometricType::Homography;
                break;
        }
    }

    if (types == cvutils::GeometricType::Undefined)
    {
        std::cout << "Using homography model because model(s) omitted or invalid"
            << std::endl;
        types = cvutils::GeometricType::Homography;
    }


    if (result.count("window") != 1 || window <= 0)
    {
        std::cout << "Using all-pair-matching because window omitted or invalid" << std::endl;
        window = 0;
    }

    if (result.count("cache") != 1)
            cacheSize = -1;
    if (result.count("prune") != 1)
        prune = false;
    if (result.count("condition") != 1)
            condition = 0;
    if (result.count("distance") != 1)
            minDist = 0;
    if (result.count("coverage") != 1)
            minCoverage = 0;
    if (result.count("symmetry") != 1)
            checkSymmetry = false;


    cv::Mat camMat;
    cv::Mat distCoeffs;
    if (!camFile.empty())
    {
        cv::FileStorage fs(camFile, cv::FileStorage::READ);
        fs["camera_matrix"] >> camMat;
        fs["distortion_coefficients"] >> distCoeffs;
    }

    cvutils::FeatureMatcher ftMatcher(inFolder, txtFile, ftFolder, isBinary, matcher,
        types, window, cacheSize, prune, condition, minDist, minCoverage, checkSymmetry,
        camMat, distCoeffs);
    ftMatcher.run();

    return 0;
}
