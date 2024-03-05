cmake_minimum_required(VERSION 3.22.1)

project(MagicPodsCore VERSION 1.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

configure_file(config/Config.h.in src/Config.h)

add_executable(MagicPodsCore 
    src/main.cpp
    src/DevicesInfoFetcher.cpp
    src/Device.cpp
    src/DeviceBattery.cpp
    src/DeviceAnc.cpp
    src/aap/Aap.cpp
    src/aap/AapClient.cpp
)
add_subdirectory(dependencies)
target_include_directories(MagicPodsCore PRIVATE src dependencies/json ${PROJECT_BINARY_DIR}/src)
target_link_libraries(MagicPodsCore PRIVATE sdbus-c++ uWebSockets bluetooth)
