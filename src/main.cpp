// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include <libuvc/libuvc.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include "eyetracking.h"

static const int kCaptureWidth = 1536;
static const int kCaptureHeight = 1024;
static const int kCaptureFPS = 60;
static const char kCurveData[8] = {250, 0, 240, 0, 250, 0, 240, 0};

/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
void cb(uvc_frame_t *frame, void *stopPtr) {
  cv::Mat cvFrame(frame->height, frame->width, CV_16UC1, frame->data);
  trackFrame(cvFrame);
}

void setLights(uvc_device_handle_t *devh, int lights) {
  uvc_set_ctrl(devh, 3, 3, (void*)(&lights), 2);
}

void setupParams(uvc_device_handle_t *devh) {
  uvc_set_ctrl(devh, 3, 4, (void*)(kCurveData), 8);
}

int main(int argc, char **argv) {
  uvc_context_t *ctx;
  uvc_device_t *dev;
  uvc_device_handle_t *devh;
  uvc_stream_ctrl_t ctrl;
  uvc_error_t res;
  /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
  res = uvc_init(&ctx, NULL);
  if (res < 0) {
    uvc_perror(res, "uvc_init");
    return res;
  }
  setupTracking();
  puts("UVC initialized");
  /* Locates the first attached UVC device, stores in dev */
  res = uvc_find_device(
      ctx, &dev,
      10667, 251, NULL); /* filter devices: vendor_id, product_id, "serial_num" */
  if (res < 0) {
    uvc_perror(res, "uvc_find_device"); /* no devices found */
  } else {
    puts("Device found");
    /* Try to open the device: requires exclusive access */
    res = uvc_open(dev, &devh);
    if (res < 0) {
      uvc_perror(res, "uvc_open"); /* unable to open device */
    } else {
      puts("Device opened");
      /* Print out a message containing all the information that libuvc
       * knows about the device */
      uvc_print_diag(devh, stderr);
      /* Try to negotiate a YUYV stream profile */
      res = uvc_get_stream_ctrl_format_size(
          devh, &ctrl, /* result stored in ctrl */
          UVC_FRAME_FORMAT_YUYV, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
          kCaptureWidth, kCaptureHeight, kCaptureFPS /* width, height, fps */
      );
      /* Print out the result */
      uvc_print_stream_ctrl(&ctrl, stderr);
      if (res < 0) {
        uvc_perror(res, "get_mode"); /* device doesn't provide a matching stream */
      } else {
        setLights(devh, 0b1111);
        setupParams(devh);
        int stop = 0;
        /* Start the video stream. The library will call user function cb:
         *   cb(frame, (void*) 0)
         */
        res = uvc_start_streaming(devh, &ctrl, cb, (void*)(&stop), 0);
        if (res < 0) {
          uvc_perror(res, "start_streaming"); /* unable to start stream */
        } else {
          puts("Streaming...");
          // uvc_set_ae_mode(devh, 1); /* e.g., turn on auto exposure */
          while(true) {
            int key = cv::waitKey(10);
            if((char)key == 'q') {
              break;
            }
            if((char)key == 'o') {
              setLights(devh, 0);
            }
            if((char)key == 'g') {
              uvc_set_gain(devh, 10);
            }
            // usleep(10000);
          }
          setLights(devh, 0);
          sleep(1);
          /* End the stream. Blocks until last callback is serviced */
          uvc_stop_streaming(devh);
          puts("Done streaming.");
        }
      }
      /* Release our handle on the device */
      uvc_close(devh);
      puts("Device closed");
    }
    /* Release the device descriptor */
    uvc_unref_device(dev);
  }
  /* Close the UVC context. This closes and cleans up any existing device handles,
   * and it closes the libusb context if one was not provided. */
  uvc_exit(ctx);
  puts("UVC exited");
  return 0;
}
