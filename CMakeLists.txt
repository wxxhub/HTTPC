cmake_minimum_required(VERSION 3.14)
project(HTTPC)

set(CMAKE_CXX_STANDARD 11)

include(FindPkgConfig)
pkg_check_modules(CURLPP REQUIRED curlpp)

add_executable(HTTPC main.cpp)
target_link_libraries(HTTPC ${CURLPP_LDFLAGS} -ljsoncpp)