## SmartGaze

A work in progress open source gaze tracking implementation, initially target at being a more robust and flexible tracking driver for the [Eye Tribe Tracker](http://theeyetribe.com/).
Based on my efforts in [reverse engineering the Eye Tribe tracker](https://github.com/trishume/EyeTribeReversing).

##Building

CMake is required to build SmartGaze

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
mkdir build
./cmakeBuild.sh
```
then open the XCode project in the build folder and run from there.

###On Windows
There is some way to use CMake on Windows but I am not familiar with it.
