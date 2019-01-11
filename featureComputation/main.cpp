#include <iostream>
#include <string>

#include "cxxopts.hpp"
#include "featureDetector.h"


int main(int argc, char** argv)
{
    std::string inFolder, outFolder;
    std::string ftFile;

    cxxopts::Options options("ftComp", "Feature computation + export helper");
    options.add_options()
        ("i,in", "image directory", cxxopts::value(inFolder))
        ("o,out", "out directory for saved features", cxxopts::value(outFolder))
        ("f,featureFile", "yml file containing the feature point info",
            cxxopts::value(ftFile));

    auto result = options.parse(argc, argv);
    if (result.count("in") != 1
        || result.count("out") != 1
        || result.count("featureFile") != 1)
    {
        std::cout << options.help() << std::endl;
        return -1;
    }

    cvutils::FeatureDetector ftDetect(inFolder, outFolder, ftFile);
    ftDetect.run();


    return 0;
}
