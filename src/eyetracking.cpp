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
static const int kEyeRegionWidth = 200;
static const int kEyeRegionHeight = 160;
static const double k8BitScale = (265.0/1024.0)*2.0;

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

static std::vector<Point> trackGlints(TrackingData *dat, Mat &m) {
  // double maxVal;
  // Point maxPt;
  // minMaxLoc(m, nullptr, &maxVal, nullptr, &maxPt);
  // std::cout << "max val: " << maxVal << " at " << maxPt << std::endl;
  // threshold(m, m, maxVal*kGlintThreshold, 255, THRESH_BINARY_INV);
  // adaptiveThreshold(m, m, 1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, -10.0);
  adaptiveThreshold(m, m, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, -40.0);

  // search for first two pixels separated sufficiently horizontally
  // start from the top and only take the first two so that glints off of teeth and headphones are ignored.
  std::vector<Point> result;
  for(int i = 0; i < m.rows; i++) {
    if(result.size() >= 2) break;
    const uint8_t* Mi = m.ptr<uint8_t>(i);
    for(int j = 0; j < m.cols; j++) {
      if(Mi[j] == 0) {
        if(result.empty()) {
          result.push_back(Point(j,i));
        } else if(j > result[0].x+kFirstGlintXShadow || j < result[0].x-kFirstGlintXShadow) {
          result.push_back(Point(j,i));
          break;
        }
      }
    }
  }
  // Make the found point more centered on the eye instead of being just the first one
  for(auto &&p : result)
    p = findLocalCenter(m,p, kGlintNeighbourhood);

  // consistent order, purely so debug views aren't jittery
  std::sort(result.begin(), result.end(), [](Point a, Point b) {
      return a.x < b.x;
  });

  return result;
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

  for(unsigned i = 0; i < glints.size(); ++i) {
    // project onto big image
    Rect roi = Rect(glints[i].x*2-(kEyeRegionWidth/2),glints[i].y*2-(kEyeRegionHeight/2),kEyeRegionWidth,kEyeRegionHeight) & Rect(0,0,bigM.cols,bigM.rows);
    Mat region(bigM, roi);
    // Rect roi = Rect(glints[i].x-(kEyeRegionWidth/4),glints[i].y-(kEyeRegionHeight/4),kEyeRegionWidth/2,kEyeRegionHeight/2) & Rect(0,0,m.cols,m.rows);
    // Mat region(m, roi);
    region.convertTo(region, CV_8U, k8BitScale, 0);
    blur(region, region, Size(3,3));
    imshow(std::to_string(i), region);
  }


  m.convertTo(m, CV_8U, k8BitScale, 0);
  Mat channels[3];
  channels[1] = m;
  channels[0] = channels[2] = min(m, glintImage);
  Mat debugImage;
  merge(channels,3,debugImage);
  // debugImage = glintImage;

  for(auto glint : glints)
    circle(debugImage, glint, 3, Scalar(255,0,255));

  imshow("main", debugImage);
}

TrackingData *setupTracking() {
  cv::namedWindow("main",CV_WINDOW_NORMAL);
  cv::namedWindow("0",CV_WINDOW_NORMAL);
  cv::namedWindow("1",CV_WINDOW_NORMAL);
  // cv::namedWindow("glint",CV_WINDOW_NORMAL);
  return new TrackingData();
}
