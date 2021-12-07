#include "io/matchesReader.h"

#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{
    cvutils::MatchesReader matchReader(argv[1]);
    auto pairWiseMatches = matchReader.moveMatches();

    std::ofstream file(argv[2], std::ofstream::out);

    for (const auto& matches : pairWiseMatches)
    {
        size_t idI = matches.first.first;
        size_t idJ = matches.first.second;

        // only for vis purposes
        if (!matches.second.empty())
            file << idI << "," << idJ << "," << matches.second.size() << std::endl;

    }
}
