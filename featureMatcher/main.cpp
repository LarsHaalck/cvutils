#include <iostream>
#include <string>

#include "cxxopts.hpp"
#include "featureMatcher.h"
#include "io/geometricType.h"

int main(int argc, char** argv)
{
    std::string inFolder, txtFile;
    std::string ftFolder;
    int matcher, window;
    int cacheSize;
    std::vector<char> geoms;

    cxxopts::Options options("ftMatch", "Feature matcher + export helper");
    options.add_options()
        ("i,in", "image folder", cxxopts::value(inFolder))
        ("t,txt", "txt file", cxxopts::value(txtFile))
        ("f,featureFile", "feature folder", cxxopts::value(ftFolder))
        ("m,matcher", "matcher method (0, 1) for (brute, flann), default 0",
            cxxopts::value(matcher))
        ("g,geometric", "geometric model for filtering (can be repeated): \
            i (isometric), s (similarity), a (affine), h (homography) default h",
            cxxopts::value(geoms))
        ("w,window", "window size (use all pairs if skipped)", cxxopts::value(window))
        ("c,cache", "cache size (if loading all into RAM is not feasable) (< 0) means \
            inf cache, (= 0) means no cache", cxxopts::value(cacheSize));

    auto result = options.parse(argc, argv);
    if (result.count("in") != 1
        || result.count("featureFile") != 1)
    {
        std::cout << options.help() << std::endl;
        return -1;
    }

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
                types |= cvutils::GeometricType::Isometric;
                break;
            case 's':
                types |= cvutils::GeometricType::Similarity;
                break;
            case 'a':
                types |= cvutils::GeometricType::Affine;
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
    {
            cacheSize = -1;
    }

    cvutils::FeatureMatcher ftMatcher(inFolder, txtFile, ftFolder, matcher, types, window,
        cacheSize);
    ftMatcher.run();

    return 0;
}
