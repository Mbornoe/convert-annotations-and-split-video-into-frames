cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(MY_GRAND_PROJECT)
find_package( OpenCV REQUIRED )

# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")

add_executable(convertAnno convertAnno.cpp)
target_link_libraries(convertAnno ${OpenCV_LIBS})

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)