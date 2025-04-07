cmake_minimum_required(VERSION 3.5.0)
project (csag)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3")

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})