cmake_minimum_required(VERSION 3.22.1)

project(MagicPodsCore VERSION 1.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

configure_file(config/Config.h.in src/Config.h)

add_executable(MagicPodsCore 
    src/main.cpp
    src/client/Client.cpp

    src/aap/setters/AapEnableNotifications.cpp
    src/aap/setters/AapInit.cpp
    src/aap/setters/AapInitExt.cpp
    src/aap/setters/AapRequest.cpp
    src/aap/setters/AapSetAnc.cpp

    src/aap/watchers/AapAncWatcher.cpp
    src/aap/watchers/AapBatteryWatcher.cpp
    src/aap/watchers/AapWatcher.cpp


    src/device/capabilities/aap/AapCapability.cpp
    src/device/capabilities/aap/AapAncCapability.cpp
    src/device/capabilities/aap/AapBatteryCapability.cpp

    src/device/capabilities/Capability.cpp
    src/device/AapDevice.cpp
    src/device/DeviceBattery.cpp    

    src/DevicesInfoFetcher.cpp
    src/Device.cpp
)
add_subdirectory(dependencies)
target_include_directories(MagicPodsCore PRIVATE src dependencies/json ${PROJECT_BINARY_DIR}/src)
target_link_libraries(MagicPodsCore PRIVATE sdbus-c++ uWebSockets bluetooth)
