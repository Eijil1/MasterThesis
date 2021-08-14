# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.17.3)

project(liveEffect)

add_library(liveEffect
		SHARED
			LiveEffectEngine.cpp
			FullDuplexStream.cpp
			FullDuplexPass.cpp
			jni_bridge.cpp
			trace.cpp)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

find_package (oboe REQUIRED CONFIG)

target_include_directories(liveEffect PUBLIC ../../libs/fftw3/include)

target_link_libraries( # Specifies the target library.
                       liveEffect

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       oboe::oboe
                       ../../libs/fftw3/lib/${ANDROID_ABI}/libfftw3.a)


















#ExternalProject_Add(fftw3
#  	URL https://www.fftw.org/fftw-3.3.9.tar.gz
#    INSTALL_DIR ${CMAKE_BINARY_DIR}
#    CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR>
#    BUILD_COMMAND make CFLAGS=-fPIC
#    LOG_DOWNLOAD 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
#    BUILD_IN_SOURCE 1
#  )

#  set(FFTW_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
#  set(FFTW_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libfftw3.a)
#ExternalProject_Get_Property(fftw3 install_dir)
#add_library(fftw STATIC IMPORTED)
#set_property(TARGET fftw PROPERTY IMPORTED_LOCATION ${install_dir}/src/fftw3/libfftw3.a)
#add_dependencies(fftw fftw3)

#include_directories(${install_dir}/include/)


#add_subdirectory(C:/Users/delso/OneDrive/Documents/Memoire/NativeTest/app/libs/test42/)
#target_include_directories(liveEffect PRIVATE C:/Users/delso/OneDrive/Documents/Memoire/NativeTest/app/libs/test42/include)
#target_link_directories(liveEffect PRIVATE C:/Users/delso/OneDrive/Documents/Memoire/NativeTest/app/libs/test3/lib/libfftw3.so)

#find_library(fftw3 NAMES fftw fftw3 lfftw3 libfftw3 PATH C:/Users/delso/OneDrive/Documents/Memoire/NativeTest/app/libs/aarch64/lib/)

#https://stackoverflow.com/questions/62772077/undefined-symbols-when-linking-fftw-fftwpp-into-project-with-cmake-on-mac
#link_directories(C:/Users/delso/OneDrive/Documents/Memoire/NativeTest/app/libs/aarch64/)
#find_library(fftw3 NAMES fftw lfftw3 libfftw3 HINTS C:/Users/delso/OneDrive/Documents/Memoire/NativeTest/app/libs/aarch64/lib)

#doc android
#add_library(fftw3 STATIC IMPORTED)
#set_target_properties( fftw3 PROPERTIES IMPORTED_LOCATION C:/Users/delso/OneDrive/Documents/Memoire/NativeTest/app/libs/aarch64/lib/libfftw3.a)

#include_directories(C:/Users/delso/OneDrive/Documents/Memoire/NativeTest/app/libs/aarch64/include)