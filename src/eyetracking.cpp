// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include "eyetracking.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;

static const float kGlintImageMultiplier = 40.0;
static const float kGlintThreshold = 0.4; // as fraction of max

static void trackGlints(Mat &m) {
  double maxVal;
  minMaxIdx(m, nullptr, &maxVal, nullptr, nullptr);
  std::cout << maxVal << std::endl;
  threshold(m, m, maxVal*kGlintThreshold, 250, THRESH_BINARY);
  imshow("glint", m);
}

void trackFrame(Mat &m) {
  Mat glintImage;
  m.convertTo(glintImage, CV_8U, (1.0/256.0)*kGlintImageMultiplier);
  trackGlints(glintImage);

  Mat debugImage;
  m.convertTo(debugImage, CV_16U, 150, 0);
  imshow("main", debugImage);
}

void setupTracking() {
  cv::namedWindow("main",CV_WINDOW_NORMAL);
  cv::namedWindow("glint",CV_WINDOW_NORMAL);
}
