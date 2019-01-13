#include <iostream>
#include <string>

#include "cxxopts.hpp"
#include "featureDetector.h"


int main(int argc, char** argv)
{
    std::string inFolder, outFolder;
    std::string txtFile, ftFile;
    float scale = 1.0f;

    cxxopts::Options options("ftComp", "Feature computation + export helper");
    options.add_options()
        ("i,in", "image directory", cxxopts::value(inFolder))
        ("t, txt", "txt file containing used images, omit if all should be used",
            cxxopts::value(txtFile))
        ("o,out", "out directory for saved features", cxxopts::value(outFolder))
        ("f,featureFile", "yml file containing the feature point info",
            cxxopts::value(ftFile))
        ("s,scale", "scale of the image (omit if scale = 1)", cxxopts::value(scale));

    auto result = options.parse(argc, argv);
    if (result.count("in") != 1
        || result.count("out") != 1
        || result.count("featureFile") != 1)
    {
        std::cout << options.help() << std::endl;
        return -1;
    }

    if (result.count("txtFile") != 1)
    {
        std::cout << "Omitted txt file. All Images in image directory will be used"
            << std::endl;
    }

    cvutils::FeatureDetector ftDetect(inFolder, outFolder, txtFile, ftFile, scale);
    ftDetect.run();


    return 0;
}
