include(FetchContent)

FetchContent_Declare(
    sdbus-cpp
    URL https://github.com/Kistler-Group/sdbus-cpp/archive/refs/tags/v1.6.0.zip
)
FetchContent_MakeAvailable(sdbus-cpp)