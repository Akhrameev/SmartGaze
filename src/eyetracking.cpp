// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include "eyetracking.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "halideFuncs.h"

using namespace cv;

static const float kGlintImageMultiplier = 40.0;
static const float kGlintThreshold = 0.35; // as fraction of max

struct TrackingData {
  HalideGens *gens;
  TrackingData() {
    gens = createGens();
  }
  ~TrackingData() {
    deleteGens(gens);
  }
};

static void trackGlints(TrackingData *dat, Mat &m) {
  double maxVal;
  // imshow("glint", m);
  minMaxIdx(m, nullptr, &maxVal, nullptr, nullptr);
  std::cout << maxVal << std::endl;
  threshold(m, m, maxVal*kGlintThreshold, 255, THRESH_BINARY_INV);
  // imshow("glint", m);
}

void trackFrame(TrackingData *dat, Mat &m) {
  // Mat glintImage;
  // m.convertTo(glintImage, CV_8U, (1.0/256.0)*kGlintImageMultiplier);
  Mat glintImage = glintKernel(dat->gens, m);
  trackGlints(dat, glintImage);

  m.convertTo(m, CV_8U, (1.0/256.0)*100.0, 0);
  Mat channels[3];
  channels[2] = m;
  channels[0] = channels[1] = min(m, glintImage);
  Mat debugImage;
  merge(channels,3,debugImage);
  imshow("main", debugImage);
}

TrackingData *setupTracking() {
  cv::namedWindow("main",CV_WINDOW_NORMAL);
  cv::namedWindow("glint",CV_WINDOW_NORMAL);
  return new TrackingData();
}
