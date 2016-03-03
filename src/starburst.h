// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text
#ifndef STARBURST_H__
#define STARBURST_H__

#include <opencv2/imgproc/imgproc.hpp>
#include <string>

extern int starThresh;
extern int starRays;
cv::RotatedRect findEllipseStarburst(cv::Mat &m, const std::string &debugName);

#endif
