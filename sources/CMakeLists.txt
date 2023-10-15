cmake_minimum_required(VERSION 3.10)
project(SnifferCLI)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Set VCPKG
set(CMAKE_TOOLCHAIN_FILE /root/vcpkg/scripts/buildsystems/vcpkg.cmake)

# Find pcapplusplus package
find_package(unofficial-pcapplusplus CONFIG REQUIRED)

# Specify the source files
set(SOURCE_FILES main.cpp sockets.cpp)

# Create the executable
add_executable(SnifferCLI ${SOURCE_FILES})

# Link pcapplusplus and other libraries to your project
target_link_libraries(SnifferCLI PRIVATE unofficial::pcapplusplus::pcappp unofficial::pcapplusplus::commonpp unofficial::pcapplusplus::packetpp)
