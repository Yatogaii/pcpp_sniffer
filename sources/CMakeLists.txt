cmake_minimum_required(VERSION 3.10)
project(SnifferCLI)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# find_package(unofficial-pcapplusplus CONFIG REQUIRED)
# find_package(pcapplusplus CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(usockets CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(pcapplusplus REQUIRED)

# Specify the source files
set(SOURCE_FILES main.cpp 
	         sockets.cpp 
		 packets.cpp 
		 protocols.cpp)

add_executable(SnifferCLI ${SOURCE_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)


# 如果有必要，链接库
target_link_libraries(SnifferCLI PRIVATE usockets::usockets)
# target_link_libraries(SnifferCLI PRIVATE uwebsockets) 
# target_link_libraries(SnifferCLI PRIVATE unofficial::pcapplusplus::pcappp unofficial::pcapplusplus::commonpp unofficial::pcapplusplus::packetpp)
target_link_libraries(SnifferCLI PRIVATE pcapplusplus::pcapplusplus)
target_link_libraries(SnifferCLI PRIVATE nlohmann_json::nlohmann_json)
# target_include_directories(SnifferCLI PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})
target_link_libraries(SnifferCLI PRIVATE ZLIB::ZLIB)

