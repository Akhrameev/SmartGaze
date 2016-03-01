// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include "halideFuncs.h"

#include "Halide.h"
using namespace Halide;

static const int kGlintKernelOffset = 8;

// template <class T> class MyGenerator : public Halide::Generator<T> {
// };

// class GlintKernelGenerator : public Generator<GlintKernelGenerator> {
class GlintKernelGenerator {
public:
    ImageParam input{UInt(16), 2, "input"};
    Var x, y;

    Func build() {
      // Define the Func.
      Func clamped = BoundaryConditions::repeat_edge(input);
      Func glints;
      Expr up = clamped(x,y-kGlintKernelOffset);
      glints(x, y) = Halide::cast<uint8_t>((clamped(x, y)*10)/up);

      // Schedule it.
      glints.vectorize(x, 8).parallel(y);

      return glints;
    }
};

struct HalideGens {
  GlintKernelGenerator glintKernelGen;
  Func glintKernelFunc;

  HalideGens() {
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

cv::Mat glintKernel(HalideGens *gens, cv::Mat &m) {
  assert(m.isContinuous());
  assert(m.type() == CV_16UC1);
  gens->glintKernelGen.input.set(Buffer(UInt(16), m.cols, m.rows, 0, 0, m.ptr()));

  cv::Mat out(m.rows, m.cols, CV_8UC1);
  Image<uint8_t> output(Buffer(UInt(8), out.cols, out.rows, 0, 0, out.ptr()));
  gens->glintKernelFunc.realize(output);

  return out;
}
