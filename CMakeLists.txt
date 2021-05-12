cmake_minimum_required(VERSION 3.16)
project(ColorObjectDetection)

set(CMAKE_CXX_STANDARD 17)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(ColorObjectDetection main.cpp ColorObject.cpp ColorObject.h)
target_link_libraries( ColorObjectDetection ${OpenCV_LIBS} )

add_executable(Hough hough.cpp)
target_link_libraries(Hough ${OpenCV_LIBS})