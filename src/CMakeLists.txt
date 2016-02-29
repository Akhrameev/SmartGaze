find_path(LIBUVC_INCLUDE_DIR libuvc/libuvc.h)
find_library(LIBUVC_LIBRARY uvc)

include_directories(${LIBUVC_INCLUDE_DIR})
link_directories(/usr/local/lib)
add_executable( SmartGaze main.cpp)
target_link_libraries( SmartGaze ${OpenCV_LIBS} ${LIBUVC_LIBRARY})
