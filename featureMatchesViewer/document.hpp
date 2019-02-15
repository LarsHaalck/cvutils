// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2017 <Zillow Inc.> Pierre Moulon

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "io/matchType.h"
#include "io/hash.h"


// Basic storage of data related to a scene
struct Document
{
    std::vector<std::string> imgFiles;
    std::vector<std::vector<cv::KeyPoint>> keyPoints;
    cv::Mat pairMat;
    std::unordered_map<std::pair<size_t, size_t>, std::vector<cv::DMatch>> pairWiseMatches;
    float scale;
    std::filesystem::path ftDir;
    cvutils::MatchType type;
};
