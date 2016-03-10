// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text
#ifndef ELLIPSE_H__
#define ELLIPSE_H__

#include <opencv2/imgproc/imgproc.hpp>

float ellipseScore(cv::Mat &m, cv::RotatedRect r);

#endif
