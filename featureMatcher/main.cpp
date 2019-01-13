#include <iostream>
#include <string>

#include "cxxopts.hpp"
#include "featureMatcher.h"


int main(int argc, char** argv)
{
    std::string inFolder, txtFile;
    std::string ftFolder;
    int matcher, window;

    cxxopts::Options options("ftMatch", "Feature matcher + export helper");
    options.add_options()
        ("i,in", "image folder", cxxopts::value(inFolder))
        ("t,txt", "txt file", cxxopts::value(txtFile))
        ("f,featureFile", "feature folder", cxxopts::value(ftFolder))
        ("m, matcher", "matcher method (0, 1) for (brute, flann), default 0",
            cxxopts::value(matcher))
        ("w, window", "window size (use all pairs if skipped)", cxxopts::value(window));

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

    if (result.count("window") != 1 || window <= 0)
    {
        std::cout << "Using all-pair-matching because window omitted or invalid" << std::endl;
        window = 0;
    }

    cvutils::FeatureMatcher ftMatcher(inFolder, txtFile, ftFolder, matcher, window);
    ftMatcher.run();

    return 0;
}
