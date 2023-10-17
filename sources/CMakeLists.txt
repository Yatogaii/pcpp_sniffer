cmake_minimum_required(VERSION 3.10)
project(SnifferCLI)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Set VCPKG
set(CMAKE_TOOLCHAIN_FILE /root/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(unofficial-pcapplusplus CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Specify the source files
set(SOURCE_FILES main.cpp 
	         sockets.cpp 
		 packets.cpp 
		 protocols.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

add_executable(SnifferCLI ${SOURCE_FILES})

target_link_libraries(SnifferCLI PRIVATE unofficial::pcapplusplus::pcappp unofficial::pcapplusplus::commonpp unofficial::pcapplusplus::packetpp)
target_link_libraries(SnifferCLI PRIVATE nlohmann_json::nlohmann_json)
