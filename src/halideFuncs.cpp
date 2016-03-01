// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include "halideFuncs.h"

#include "Halide.h"
using namespace Halide;


// template <class T> class MyGenerator : public Halide::Generator<T> {
// };

// class GlintKernelGenerator : public Generator<GlintKernelGenerator> {
class GlintKernelGenerator {
  static const int kKernelOffset = 8;
public:
  ImageParam input{UInt(16), 2, "input"};
  Var x, y;

  Func build() {
    // Define the Func.
    Func clamped = BoundaryConditions::repeat_edge(input);
    Func out;
    Expr up = clamped(x,y-kKernelOffset);
    out(x, y) = Halide::cast<uint8_t>((clamped(x, y)*10)/up);

    // Schedule it.
    out.vectorize(x, 8).parallel(y);

    return out;
  }
};

class FindGlintsGenerator {
public:
  static const int kBoxSize = 32;
  static const int kThresh = 3;
  ImageParam input{UInt(8), 2, "input"};
  Var x, y;

  Func build() {
    // Define the Func.
    RDom xr(0, kBoxSize);
    RDom yr(0, kBoxSize);
    Func out;

    out(x, y) = select(sum(yr, sum(xr, input(x*kBoxSize+xr,y*kBoxSize+yr))) > kThresh, Halide::cast<uint8_t>(255), Halide::cast<uint8_t>(0));

    out.vectorize(x, 16);

    return out;
  }
};

struct HalideGens {
  GlintKernelGenerator glintKernelGen;
  Func glintKernelFunc;

  FindGlintsGenerator findGlintsGen;
  Func findGlintsFunc;

  HalideGens() {
    findGlintsFunc = findGlintsGen.build();
    findGlintsFunc.compile_jit();
    glintKernelFunc = glintKernelGen.build();
    glintKernelFunc.compile_jit();
  }
};

HalideGens *createGens() {
  return new HalideGens();
}
void deleteGens(HalideGens *gens) {
  delete gens;
}

static cv::Mat runFunc(Func &f, ImageParam &inParam, cv::Mat &m, cv::Size outSize) {
  assert(m.isContinuous());

  inParam.set(Buffer(UInt(m.elemSize()*8), m.cols, m.rows, 0, 0, m.ptr()));

  cv::Mat out(outSize, CV_8UC1);
  Image<uint8_t> output(Buffer(UInt(8), out.cols, out.rows, 0, 0, out.ptr()));
  f.realize(output);

  return out;
}

cv::Mat glintKernel(HalideGens *gens, cv::Mat &m) {
  assert(m.type() == CV_16UC1);
  return runFunc(gens->glintKernelFunc, gens->glintKernelGen.input, m, m.size());
}

cv::Mat findGlints(HalideGens *gens, cv::Mat &m) {
  assert(m.type() == CV_8UC1);
  cv::Size outSize(m.cols/FindGlintsGenerator::kBoxSize, m.rows/FindGlintsGenerator::kBoxSize);
  return runFunc(gens->findGlintsFunc, gens->findGlintsGen.input, m, outSize);
}
