cmake_minimum_required(VERSION 3.18)
project(pyrocks11 VERSION 0.1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)

# Set RocksDB source directory
set(ROCKSDB_SOURCE_DIR "${CMAKE_SOURCE_DIR}/extern/rocksdb")

# Verify RocksDB source exists
if(NOT EXISTS "${ROCKSDB_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "RocksDB source not found at ${ROCKSDB_SOURCE_DIR}. Please clone RocksDB repository to extern/rocksdb")
endif()

# RocksDB build options
set(WITH_GFLAGS OFF CACHE BOOL "Build with GFlags")
set(WITH_SNAPPY OFF CACHE BOOL "Build with Snappy")
set(WITH_LZ4 ON CACHE BOOL "Build with LZ4")
set(WITH_ZLIB ON CACHE BOOL "Build with ZLIB")
set(WITH_ZSTD ON CACHE BOOL "Build with ZSTD")
set(WITH_BZ2 OFF CACHE BOOL "Build with BZ2")
set(WITH_TESTS OFF CACHE BOOL "Build RocksDB tests")
set(WITH_TOOLS OFF CACHE BOOL "Build RocksDB tools")
set(USE_RTTI ON CACHE BOOL "RTTI is needed for Python bindings to work")
set(ROCKSDB_BUILD_SHARED OFF CACHE BOOL "Build shared library")
set(ROCKSDB_INSTALL OFF CACHE BOOL "Install RocksDB")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(${ROCKSDB_SOURCE_DIR})

# Add C++ source directory
add_subdirectory(src/cpp)

# Print information for debugging
message(STATUS "Python_LIBRARIES: ${Python_LIBRARIES}")
message(STATUS "PYTHON_SITE_PACKAGES: ${PYTHON_SITE_PACKAGES}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
