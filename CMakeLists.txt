cmake_minimum_required(VERSION 3.22.1)

project(MagicPodsCore LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(MagicPodsCore 
    src/main.cpp
    src/DevicesInfoFetcher.cpp
    src/Device.cpp
    src/DeviceBattery.cpp
    src/aap/Aap.cpp
    src/aap/AapClient.cpp
)
add_subdirectory(dependencies)
target_include_directories(MagicPodsCore PRIVATE src dependencies/json)
target_link_libraries(MagicPodsCore PRIVATE sdbus-c++ uWebSockets bluetooth -static-libgcc -static-libstdc++)