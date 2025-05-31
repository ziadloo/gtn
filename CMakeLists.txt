cmake_minimum_required(VERSION 3.18...3.27)

project(gtn LANGUAGES CXX)

find_program(NVCC_EXECUTABLE nvcc HINTS /usr/local/cuda/bin $ENV{CUDA_HOME}/bin)

if(NVCC_EXECUTABLE)
  enable_language(CUDA)
  include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
endif()

if(NVCC_EXECUTABLE)
  enable_language(CUDA)
  set(CUDA_AVAILABLE ON)
else()
  set(CUDA_AVAILABLE OFF)
  message(STATUS "CUDA_AVAILABLE IS OFF")
endif()

# ----------------------------- Setup -----------------------------
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

# ----------------------------- Configuration -----------------------------
option(GTN_BUILD_TESTS "Build tests for gtn" ON)
option(GTN_BUILD_BENCHMARKS "Build benchmarks for gtn" ON)
option(GTN_BUILD_EXAMPLES "Build examples for gtn" ON)
option(GTN_BUILD_CUDA "Build cuda backend for gtn" ${CUDA_AVAILABLE})
option(GTN_BUILD_PYTHON_BINDINGS "Build python bindings for gtn" OFF)

# ----------------------------- Dependencies -----------------------------
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

message(STATUS "CUDA_AVAILABLE: ${CUDA_AVAILABLE} GTN_BUILD_CUDA: ${GTN_BUILD_CUDA}")

# Determine if CUDA is available
if(GTN_BUILD_CUDA AND NOT CUDA_AVAILABLE)
  message(FATAL_ERROR "Requested CUDA but not found, halting the process")
  return()
endif()

if (GTN_BUILD_CUDA)
  include_directories(${CUDA_INCLUDE_DIRS})
  add_definitions(-D_CUDA_)
  enable_language(CUDA)
  set(CMAKE_CUDA_STANDARD 14)
  set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()

# ----------------------------- Lib -----------------------------
# Core lib sources -- maybe make me a submodule eventually
include(${CMAKE_SOURCE_DIR}/gtn/CMakeLists.txt)

if (GTN_BUILD_CUDA)
  set(CMAKE_CUDA_ARCHITECTURES 80 86 89)
  add_compile_options(
    $<$<COMPILE_LANGUAGE:CUDA>:-default-stream=per-thread>
    $<$<COMPILE_LANGUAGE:CUDA>:--extended-lambda>)
endif()

# Main lib
add_library(
  gtn
  ${GTN_SOURCES}
)

if (GTN_BUILD_CUDA)
  target_sources(gtn PRIVATE ${GTN_CUDA_SOURCES})
else()
  target_sources(gtn PRIVATE ${GTN_NO_CUDA_SOURCES})
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/gtn)

target_link_libraries(gtn PUBLIC ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(
  gtn
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)

# Python Bindings
if (GTN_BUILD_PYTHON_BINDINGS)
  message(STATUS "Building Python bindings.")
  include(${CMAKE_MODULE_PATH}/Buildpybind11.cmake)

  add_subdirectory(${PROJECT_SOURCE_DIR}/bindings/python)
endif()

# Tests
if (GTN_BUILD_TESTS)
  enable_testing()
  add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()

# Benchmarks
if (GTN_BUILD_BENCHMARKS)
  add_subdirectory(${PROJECT_SOURCE_DIR}/benchmarks)
endif()

# Examples
if (GTN_BUILD_EXAMPLES)
  add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()


# ----------------------------- Installation -----------------------------
# Installation directories
set(GTN_INSTALL_INC_DIR "include" CACHE PATH "Install path for headers")
set(GTN_INSTALL_INC_DIR_HEADER_LOC ${GTN_INSTALL_INC_DIR}/gtn)
set(GTN_INSTALL_LIB_DIR "lib" CACHE PATH "Install path for libraries")
set(GTN_INSTALL_BIN_DIR "bin" CACHE PATH "Install path for binaries")
# Other assets
set(GTN_INSTALL_ASSETS_BASE_DIR "share/gtn")
set(GTN_INSTALL_CMAKE_DIR "${GTN_INSTALL_ASSETS_BASE_DIR}/cmake" CACHE PATH "Install path for CMake files")

# Main target
install(
  TARGETS gtn
  EXPORT gtnTargets
  COMPONENT gtn
  PUBLIC_HEADER DESTINATION gtn
  RUNTIME DESTINATION ${GTN_INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${GTN_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${GTN_INSTALL_LIB_DIR}
  FRAMEWORK DESTINATION framework
  INCLUDES DESTINATION ${GTN_INSTALL_INC_DIR}
)

# Write and install targets file
install(
  EXPORT
  gtnTargets
  NAMESPACE
  gtn::
  DESTINATION
  ${GTN_INSTALL_CMAKE_DIR}
  COMPONENT
  cmake)

# Move headers
install(
  DIRECTORY
  ${CMAKE_SOURCE_DIR}/gtn/ # module headers in ./gtn
  COMPONENT
  headers
  DESTINATION
  ${GTN_INSTALL_INC_DIR_HEADER_LOC}
  FILES_MATCHING # preserve directory structure
  PATTERN  "*.h"
  )

# Write config file
include(CMakePackageConfigHelpers)
set(INCLUDE_DIRS include)
set(CMAKE_DIR ${GTN_INSTALL_CMAKE_DIR})
configure_package_config_file(
  ${CMAKE_MODULE_PATH}/gtnConfig.cmake.in
  cmake/install/${GTN_CONFIG_CMAKE_BUILD_DIR}/gtnConfig.cmake
  INSTALL_DESTINATION
  ${GTN_INSTALL_CMAKE_DIR}
  PATH_VARS INCLUDE_DIRS CMAKE_DIR
  )
install(FILES
  ${PROJECT_BINARY_DIR}/cmake/install/gtnConfig.cmake
  DESTINATION ${GTN_INSTALL_CMAKE_DIR}
  COMPONENT cmake
  )
