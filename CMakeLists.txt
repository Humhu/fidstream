# FiducialStream CMake files
# Humphrey Hu, 2014

project(FiducialStream)

cmake_minimum_required(VERSION 2.8)

# Defaults to release build
if (NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif ()
message(STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")

# Set install paths
set (LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib" CACHE PATH
  "Target directory for all libraries.")
set (EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin" CACHE PATH
  "Target directory for all executables.")

# Can only use C++0x in Ubuntu 12.04 GCC 4.6
add_definitions(-Wall -std=c++0x)

set(FiducialStream_VERSION_MAJOR 1)
set(FiducialStream_VERSION_MINOR 0)
set(FiducialStream_VERSION "${FiducialStream_VERSION_MAJOR}.${FiducialStream_VERSION_MINOR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Linear algebra library
find_package(Eigen REQUIRED)
include_directories(${EIGEN_INCLUDE_DIRS})
list(APPEND FiducialStream_LIBRARIES ${EIGEN_LIBRARIES})

# AprilTags library
find_package(AprilTags REQUIRED
	HINTS ~/Software/apriltags2/install)
message(STATUS "AprilTags found at: ${AprilTags_INCLUDE_DIRECTORIES}")
include_directories(${AprilTags_INCLUDE_DIRECTORIES})
list(APPEND FiducialStream_LIBRARIES ${AprilTags_LIBRARIES})

# OpenCV imaging
# OpenCV for camera interface
find_package( OpenCV REQUIRED
    HINTS ~/Software/opencv/install )
message(STATUS "OpenCV found at: ${OpenCV_INCLUDE_DIRS}")
include_directories(${OpenCV_INCLUDE_DIRS})
list(APPEND FiducialStream_LIBRARIES ${OpenCV_LIBS})
if( OpenCV_VERSION VERSION_EQUAL "3")
	add_definitions("-DOPENCV3=1")
endif()

# Boost
find_package(Boost REQUIRED
	COMPONENTS thread system )
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND FiducialStream_LIBRARIES ${Boost_LIBRARIES})

# Project headers
include_directories("${PROJECT_SOURCE_DIR}/include")

set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/fidstr")
set(LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(BINARY_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")

add_subdirectory(src)
add_subdirectory(apps)

export(TARGETS fidstr
    FILE "${PROJECT_BINARY_DIR}/FiducialStreamTargets.cmake")

configure_file(FiducialStreamConfig.cmake.in
    "${PROJECT_BINARY_DIR}/FiducialStreamConfig.cmake" @ONLY)

configure_file(FiducialStreamConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/FiducialStreamConfigVersion.cmake" @ONLY)

install(FILES
    "${PROJECT_BINARY_DIR}/FiducialStreamConfig.cmake"
    "${PROJECT_BINARY_DIR}/FiducialStreamConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_PREFIX}" COMPONENT dev)

install(EXPORT FiducialStreamTargets
    DESTINATION "${CMAKE_INSTALL_PREFIX}" COMPONENT dev)
