cmake_minimum_required(VERSION 3.10)
project(ObjectDetection)

include(CTest)
enable_testing()

# Put your OpenCV build directory here (Use the top level path (C:/))
set(OpenCV_DIR "C:/Users/theli/Documents/Visual-Studio-Projects/OPENCV/opencv/build")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Decide which executable files you want here.
# add_executable(ObjectDetection face-recognition-opencv.cpp)
add_executable(ObjectDetection ball-recog-opencv.cpp)
# add_executable (ObjectDetection cube-recog-opencv.cpp)

target_link_libraries(ObjectDetection ${OpenCV_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)