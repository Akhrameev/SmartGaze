// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text
#ifndef EYETRACKING_H__
#define EYETRACKING_H__

#include <opencv2/imgproc/imgproc.hpp>

struct TrackingData;

TrackingData *setupTracking();
void trackFrame(TrackingData *dat, cv::Mat &m);

#endif

