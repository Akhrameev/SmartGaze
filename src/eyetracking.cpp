// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include "eyetracking.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <chrono>
#include <utility>

#include "halideFuncs.h"

static const int kFirstGlintXShadow = 100;
static const int kGlintNeighbourhood = 100;

using namespace cv;

struct TrackingData {
  HalideGens *gens;
  TrackingData() {
    gens = createGens();
  }
  ~TrackingData() {
    deleteGens(gens);
  }
};

// search for other set pixels in an area around the point and find the average of the set locations
static Point findLocalCenter(Mat &m, Point p, int size) {
  int xSum = 0;
  int ySum = 0;
  int count = 0;
  for(int i = std::max(0,p.y-size); i < std::min(m.rows,p.y+size); i++) {
    const uint8_t* Mi = m.ptr<uint8_t>(i);
    for(int j = std::max(0,p.x-size); j < std::min(m.cols,p.x+size); j++) {
      if(Mi[j] == 0) {
        xSum += j; ySum += i;
        count += 1;
      }
    }
  }
  if(count == 0) return Point(0,0);
  return Point(xSum/count, ySum/count);
}

static std::pair<Point,Point> trackGlints(TrackingData *dat, Mat &m) {
  // double maxVal;
  // Point maxPt;
  // minMaxLoc(m, nullptr, &maxVal, nullptr, &maxPt);
  // std::cout << "max val: " << maxVal << " at " << maxPt << std::endl;
  // threshold(m, m, maxVal*kGlintThreshold, 255, THRESH_BINARY_INV);
  // adaptiveThreshold(m, m, 1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, -10.0);
  adaptiveThreshold(m, m, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, -50.0);

  // search for first two pixels separated sufficiently horizontally
  // start from the top and only take the first two so that glints off of teeth and headphones are ignored.
  Point firstPt(0,0), secondPt(0,0);
  bool foundFirst = false;
  bool foundSecond = false;
  for(int i = 0; i < m.rows; i++) {
    if(foundSecond) break;
    const uint8_t* Mi = m.ptr<uint8_t>(i);
    for(int j = 0; j < m.cols; j++) {
      if(Mi[j] == 0) {
        if(!foundFirst) {
          firstPt = Point(j,i);
          foundFirst = true;
        } else if(j > firstPt.x+kFirstGlintXShadow || j < firstPt.x-kFirstGlintXShadow) {
          secondPt = Point(j,i);
          foundSecond = true;
          break;
        }
      }
    }
  }
  // Make the found point more centered on the eye instead of being just the first one
  return std::pair<Point,Point>(
    findLocalCenter(m,firstPt, kGlintNeighbourhood),
    findLocalCenter(m,secondPt, kGlintNeighbourhood));
}

void trackFrame(TrackingData *dat, Mat &bigM) {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  start = std::chrono::high_resolution_clock::now();

  // fix stuck pixel on my EyeTribe by pasting over it
  // TODO: don't enable this for everyone else
  bigM.at<uint16_t>(283,627) = bigM.at<uint16_t>(283,626);

  Mat m;
  resize(bigM, m, Size(bigM.cols/2,bigM.rows/2));

  Mat glintImage;
  m.convertTo(glintImage, CV_8U, 256.0/1024.0);
  // glintImage = glintKernel(dat->gens, m);
  auto glints = trackGlints(dat, glintImage);
  Mat foundGlints = findGlints(dat->gens, glintImage);

  end = std::chrono::high_resolution_clock::now();
  std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << "ms\n";

  m.convertTo(m, CV_8U, (1.0/256.0)*100.0, 0);
  Mat channels[3];
  channels[1] = m;
  channels[0] = channels[2] = min(m, glintImage);
  Mat debugImage;
  merge(channels,3,debugImage);
  // debugImage = glintImage;

  circle(debugImage, glints.first, 3, Scalar(255,0,255));
  circle(debugImage, glints.second, 3, Scalar(255,0,255));

  imshow("main", debugImage);
}

TrackingData *setupTracking() {
  cv::namedWindow("main",CV_WINDOW_NORMAL);
  // cv::namedWindow("glint",CV_WINDOW_NORMAL);
  return new TrackingData();
}
