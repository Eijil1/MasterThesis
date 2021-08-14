project(EnvEnh)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 11)

add_executable(EnvEnh main.cpp ee.cpp ee.h Wiener.cpp Wiener.h)

target_include_directories(EnvEnh PUBLIC fftw-3.3.5-dll64)
target_link_directories(EnvEnh PUBLIC fftw-3.3.5-dll64)
target_link_libraries(EnvEnh fftw3-3)
