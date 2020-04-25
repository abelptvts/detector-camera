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

include_directories('${CMAKE_SOURCE_DIR}/deps/socket.io-client-cpp/build/include')
add_library(sioclient STATIC IMPORTED)
set_target_properties(sioclient PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/socket.io-client-cpp/build/lib/Release/libsioclient.a)
add_library(sioclient-tls STATIC IMPORTED)
set_target_properties(sioclient-tls PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/socket.io-client-cpp/build/lib/Release/libsioclient_tls.a)
add_library(boost-date STATIC IMPORTED)
set_target_properties(boost-date PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/socket.io-client-cpp/build/lib/Release/libboost_date_time.a)
add_library(boost-random STATIC IMPORTED)
set_target_properties(boost-random PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/socket.io-client-cpp/build/lib/Release/libboost_random.a)
add_library(boost-system STATIC IMPORTED)
set_target_properties(boost-system PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/socket.io-client-cpp/build/lib/Release/libboost_system.a)


file(GLOB SRC "src/*.cpp" "src/*.h" "src/**/*.cpp" "src/**/*.h" "src/**/*.tpp" "src/**/*.hpp")
add_executable(detector_camera ${SRC})
if (${CMAKE_CXX_COMPILER} MATCHES arm)
    target_link_libraries(detector_camera tensorflow opencv-core opencv-imgcodecs opencv-imgproc opencv-videoio pthread sioclient sioclient-tls boost-date boost-random boost-random boost-system)
else ()
    target_link_libraries(detector_camera tensorflow ${OpenCV_LIBS} pthread sioclient sioclient-tls boost-date boost-random boost-random boost-system)
endif ()

install(TARGETS detector_camera RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION /var/lib/detector_camera/models)
install(FILES ${CMAKE_SOURCE_DIR}/detector_camera.conf DESTINATION /etc/detector_camera/)
install(FILES ${CMAKE_SOURCE_DIR}/detector_camera.service DESTINATION /lib/systemd/system)

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libopencv-videoio3.2 (>= 3.2.0), libssl1.1 (>= 1.1)")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Abel Putovits - Detector Camera")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Abel Putovits")
set(CPACK_PACKAGE_DESCRIPTION "Detector Camera")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "putovici.abel@gmail.com")
set(CPACK_PACKAGE_FILE_NAME "detector_camera-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

if (${CMAKE_CXX_COMPILER} MATCHES arm)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
endif ()

include(CPack)
