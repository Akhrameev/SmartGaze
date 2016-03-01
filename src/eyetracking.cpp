// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include "eyetracking.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <chrono>

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
  // double maxVal;
  // minMaxIdx(m, nullptr, &maxVal, nullptr, nullptr);
  // threshold(m, m, maxVal*kGlintThreshold, 255, THRESH_BINARY_INV);
  adaptiveThreshold(m, m, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, -10.0);
}

void trackFrame(TrackingData *dat, Mat &m) {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  start = std::chrono::high_resolution_clock::now();

  Mat glintImage;
  // m.convertTo(glintImage, CV_8U, (1.0/256.0)*kGlintImageMultiplier);
  glintImage = glintKernel(dat->gens, m);
  trackGlints(dat, glintImage);

  end = std::chrono::high_resolution_clock::now();
  std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << "ms\n";

  m.convertTo(m, CV_8U, (1.0/256.0)*100.0, 0);
  Mat channels[3];
  channels[1] = m;
  channels[0] = channels[2] = min(m, glintImage);
  Mat debugImage;
  merge(channels,3,debugImage);
  imshow("main", debugImage);
}

TrackingData *setupTracking() {
  cv::namedWindow("main",CV_WINDOW_NORMAL);
  // cv::namedWindow("glint",CV_WINDOW_NORMAL);
  return new TrackingData();
}
