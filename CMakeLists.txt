cmake_minimum_required(VERSION 3.22.1)

project(MagicPodsCore VERSION 2.0.2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

configure_file(config/Config.h.in src/Config.h)

add_executable(MagicPodsCore 
    src/main.cpp

    src/tests/TestsSgb.cpp

    src/client/Client.cpp

    src/sdk/aap/AapHelper.cpp
    src/sdk/aap/setters/AapEnableNotifications.cpp
    src/sdk/aap/setters/AapInit.cpp
    src/sdk/aap/setters/AapInitExt.cpp
    src/sdk/aap/setters/AapRequest.cpp
    src/sdk/aap/setters/AapSetAnc.cpp
    src/sdk/aap/setters/AapSetConversationAwarenessStatus.cpp
    src/sdk/aap/setters/AapSetNoiseCancellationOneAirPod.cpp
    src/sdk/aap/setters/AapSetPressAndHoldDuration.cpp
    src/sdk/aap/setters/AapSetPressSpeed.cpp

    src/sdk/aap/watchers/AapAncWatcher.cpp
    src/sdk/aap/watchers/AapBatteryWatcher.cpp
    src/sdk/aap/watchers/AapWatcher.cpp
    src/sdk/aap/watchers/AapConversationAwarenessWatcher.cpp
    src/sdk/aap/watchers/AapConversationAwarenessStateWatcher.cpp
    src/sdk/aap/watchers/AapNoiseCancellationOneAirPodWatcher.cpp
    src/sdk/aap/watchers/AapPressAndHoldDurationWatcher.cpp
    src/sdk/aap/watchers/AapPressSpeedWatcher.cpp

    src/sdk/sgb/watchers/GalaxyBudsAncWatcher.cpp
    src/sdk/sgb/watchers/GalaxyBudsBatteryWatcher.cpp
    src/sdk/sgb/watchers/GalaxyBudsEarDetectionWatcher.cpp
    src/sdk/sgb/Crc16.cpp
    src/sdk/sgb/GalaxyBudsHelper.cpp
    src/sdk/sgb/GalaxyBudsPacket.cpp

    src/device/capabilities/aap/AapCapability.cpp
    src/device/capabilities/aap/AapAncCapability.cpp
    src/device/capabilities/aap/AapBatteryCapability.cpp
    src/device/capabilities/aap/AapConversationAwarenessCapability.cpp
    src/device/capabilities/aap/AapConversationAwarenessStateCapability.cpp
    src/device/capabilities/aap/AapNoiseCancellationOneAirPodModeCapability.cpp
    src/device/capabilities/aap/AapPressAndHoldDurationCapability.cpp
    src/device/capabilities/aap/AapPressSpeedCapability.cpp
    
    src/device/capabilities/sgb/GalaxyBudsCapability.cpp
    src/device/capabilities/sgb/GalaxyBudsAncCapability.cpp
    src/device/capabilities/sgb/GalaxyBudsBatteryCapability.cpp

    src/device/capabilities/Capability.cpp
    src/device/AapDevice.cpp
    src/device/GalaxyBudsDevice.cpp
    src/device/DeviceBattery.cpp    
    src/device/Device.cpp

    src/dbus/DBusDeviceInfo.cpp
    src/dbus/DBusService.cpp

    src/DevicesInfoFetcher.cpp
)
add_subdirectory(dependencies)
target_include_directories(MagicPodsCore PRIVATE
    src    
    dependencies/json
    ${PROJECT_BINARY_DIR}/src
)
target_link_libraries(MagicPodsCore PRIVATE sdbus-c++ uWebSockets bluetooth)
