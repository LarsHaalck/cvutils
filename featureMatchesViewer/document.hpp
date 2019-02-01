// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2017 <Zillow Inc.> Pierre Moulon

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "io.h"


// Basic storage of data related to a scene
struct Document
{
    std::vector<std::string> imgFiles;
    std::vector<std::vector<cv::KeyPoint>> keyPoints;
    cv::Mat pairMat;
    std::vector<std::vector<cv::DMatch>> matches;
    float scale;
    std::filesystem::path ftDir;
    cvutils::detail::MatchType type;

    int getMatchRow(size_t i, size_t j);
    void eraseMatchRow(size_t k);
};
