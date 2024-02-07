cmake_minimum_required(VERSION 3.22.1)

project(MagicPodsCore LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(MagicPodsCore 
    src/main.cpp
    src/DevicesInfoFetcher.cpp
    src/Device.cpp
    src/aap/aap.cpp
)
add_subdirectory(dependencies)
target_include_directories(MagicPodsCore PRIVATE src src/aap dependencies/json)
target_link_libraries(MagicPodsCore PRIVATE sdbus-c++ uWebSockets bluetooth)