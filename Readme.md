## SmartGaze

A work in progress open source gaze tracking implementation, initially target at being a more robust and flexible tracking driver for the [Eye Tribe Tracker](http://theeyetribe.com/).
Based on my efforts in [reverse engineering the Eye Tribe tracker](https://github.com/trishume/EyeTribeReversing).

##Building

CMake is required to build SmartGaze. You will also need the [libuvc](https://github.com/ktossell/libuvc) library for camera feed capture and OpenCV and [Halide](http://halide-lang.org/)
installed for image processing.

###OSX or Linux with Make
```bash
# do things in the build directory so that we don't clog up the main directory
mkdir build
cd build
cmake ../
make
./bin/eyeLike # the executable file
```

###On OSX with XCode
```bash
# Install dependencies
brew install libuvc
# I use brew install libuvc --HEAD but that shouldn't be necessary

brew tap homebrew/science
brew install homebrew/science/opencv

brew tap halide/homebrew-halide
brew install halide/halide/halide

mkdir build
./cmakeBuild.sh
```
then open the XCode project in the build folder and run from there.

###On Windows
There is some way to use CMake on Windows but I am not familiar with it.

## License

This software is licensed under the GPLv2 (see the `LICENSE` file). The reason I didn't choose a permissive license is that I wrote this
software specifically because I was dissapointed by the inflexibility and poor performance of closed source
eye tracking software. Supposing that I succeed in making a higher quality eye tracking implementation than
proprietary software, I don't want any eye tracking companies to be able to use this software without also
making any modifications to it open source.

SmartGaze is also a program not a library so I gain no adoption benefits by allowing linking to proprietary code.

The first couple commits of camera capture code before I wrote any computer vision were released under the MIT licence though.
