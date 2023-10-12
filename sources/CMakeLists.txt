cmake_minimum_required(VERSION 3.10)
project(SnifferCLI)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 14)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

# Include the libpcap headers
include_directories(${PCAP_INCLUDE_DIRS})

# Specify the source files
set(SOURCE_FILES main.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Create the executable
add_executable(SnifferCLI ${SOURCE_FILES})

# Link the executable to the necessary libraries
target_link_libraries(SnifferCLI ${PCAP_LIBRARIES})

