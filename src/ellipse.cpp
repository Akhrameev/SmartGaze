// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text
#include "ellipse.h"

#include <opencv2/highgui/highgui.hpp>
#include <cmath>
#include <cstdint>

using namespace cv;

#include "ellipse2Poly.h"

// Modified from http://stackoverflow.com/questions/13299409/how-to-get-the-image-pixel-at-real-locations-in-opencv
static uint8_t getPixelSubpix(const cv::Mat& img, cv::Point2f pt) {
  cv::Mat patch;
  cv::getRectSubPix(img, cv::Size(1,1), pt, patch);
  return patch.at<uint8_t>(0,0);
}

static float ellipseContourIntegral(Mat &m, const RotatedRect &r) {
  std::vector<Point2f> pts;
  ellipsePoints(r, 0, 360, 2, pts);

  // for(Point2f p : pts) {
  //   Point p2(cvRound(p.x), cvRound(p.y));
  //   circle(m, p2, 0, Scalar(0));
  // }
}

float ellipseScore(Mat &m, RotatedRect r) {
  ellipseContourIntegral(m, r);
}
