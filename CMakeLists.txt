cmake_minimum_required(VERSION 3.10)
project(detector_camera)

set(CMAKE_CXX_STANDARD 11)

add_library(tensorflow STATIC IMPORTED)
include_directories(${CMAKE_SOURCE_DIR}/deps/tensorflow-2.0.0/ ${CMAKE_SOURCE_DIR}/deps/tensorflow-2.0.0/tensorflow/lite/tools/make/downloads/absl ${CMAKE_SOURCE_DIR}/deps/tensorflow-2.0.0/tensorflow/lite/tools/make/downloads/flatbuffers/include)
if (${CMAKE_CXX_COMPILER} MATCHES arm)
    set_target_properties(tensorflow PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/tensorflow-2.0.0/tensorflow/lite/tools/make/gen/rpi_armv7l/lib/libtensorflow-lite.a)
else ()
    set_target_properties(tensorflow PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/tensorflow-2.0.0/tensorflow/lite/tools/make/gen/linux_x86_64/lib/libtensorflow-lite.a)
endif ()

if (${CMAKE_CXX_COMPILER} MATCHES arm)
    include_directories(/usr/include/opencv2)
    add_library(opencv-videoio SHARED IMPORTED)
    set_target_properties(opencv-videoio PROPERTIES IMPORTED_LOCATION /usr/lib/arm-linux-gnueabihf/libopencv_videoio.so)

    add_library(opencv-imgproc SHARED IMPORTED)
    set_target_properties(opencv-imgproc PROPERTIES IMPORTED_LOCATION /usr/lib/arm-linux-gnueabihf/libopencv_imgproc.so)

    add_library(opencv-imgcodecs SHARED IMPORTED)
    set_target_properties(opencv-imgcodecs PROPERTIES IMPORTED_LOCATION /usr/lib/arm-linux-gnueabihf/libopencv_imgcodecs.so)

    add_library(opencv-core SHARED IMPORTED)
    set_target_properties(opencv-core PROPERTIES IMPORTED_LOCATION /usr/lib/arm-linux-gnueabihf/libopencv_core.so)
else ()
    find_package(OpenCV COMPONENTS core imgproc imgcodecs videoio REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif ()

file(GLOB SRC "src/*.cpp")
add_executable(detector_camera ${SRC})
if (${CMAKE_CXX_COMPILER} MATCHES arm)
    target_link_libraries(detector_camera tensorflow opencv-core opencv-imgcodecs opencv-imgproc opencv-videoio pthread)
else ()
    target_link_libraries(detector_camera tensorflow ${OpenCV_LIBS} pthread)
endif ()
