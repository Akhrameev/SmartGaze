// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include "starburst.h"

#include <opencv2/highgui/highgui.hpp>

using namespace cv;

RotatedRect findEllipseStarburst(Mat &m, const std::string &debugName) {
  imshow(debugName, m);
  return RotatedRect();
}
