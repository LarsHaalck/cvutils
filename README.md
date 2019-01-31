# OpenCV-Utilities
This repository supplies four handy tools for fast prototyping of 
feature based projects.

The provided tools are:
# Feature Detector
```
Feature computation + export helper
Usage:
  ftComp [OPTION...]

  -i, --in arg           image directory
  -t, --txt arg          txt file containing used images, omit if all should
                         be used
  -o, --out arg          out directory for saved features
  -f, --featureFile arg  yml file containing the feature point info
  -s, --scale arg        scale of the image (omit if scale = 1)
```
This tools detects features in all images in a supplied directory. When give a txt file
it selects only those files (every line is a single file relative to the image dir).

The resulting feature files are written to the out directory. 

A scale can be supplied if the images have to be scaled **before** retrieveing the
features. 

The type of feature is determined by the supplied feature file (see examples directory). 
Currenly supported types: **ORB**, **SIFT**

# Feature Matcher
```
Feature matcher + export helper
Usage:
  ftMatch [OPTION...]

  -i, --in arg           image folder
  -t, --txt arg          txt file
  -f, --featureFile arg  feature folder
  -m, --matcher arg      matcher method (0, 1) for (brute, flann), default 0
  -w, --window arg       window size (use all pairs if skipped)
```
This tool matches the features from the image folder using brute force or OpenCV's
knn-based Flann matcher. This can either be done for every image pair or in a sliding
window.


# Feature Viewer
This GUI application visualizes the detected features

# Feature Matcher
This GUI application visualizes the matches between frames and is heavily based on
the matches viewer of OpenMVG (https://github.com/openMVG/openMVG) but allows the
manual deletion of wrong matches by hand. 

# Dependencies
* OpenCV (>= 4.0)
* Qt (>= 5.0)
* CMake (>= 3.8)

# Implicit dependencies
The following small header only libraries are used and directly included. 
No installing is necessary
* cxxopts (https://github.com/jarro2783/cxxopts)
* cpptqdm (https://github.com/aminnj/cpptqdm)

# Quick Start
```
mkdir build 
cd build
cmake -DCMAKE_BUILD_TYPE=Relase -DCMAKE_INSTALL_PREFIX=<someInstallDir> ../
make 
make install
```

The executables are then installed to ``<someInstallDir>/bin``
