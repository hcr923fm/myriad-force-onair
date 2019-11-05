cmake_minimum_required(VERSION 3.7)
set(CMAKE_SYSTEM_VERSION $ENV{WIN_API_VERSION})
project(force-myriad-onair CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories("$PROJECT_SOURCE_DIR"
"C:/Advantech/DAQNavi/Inc"
)

add_executable(force-myriad-onair
force-onair.cpp
myriad-controller.cpp
gpio-handler.cpp)

target_link_libraries(force-myriad-onair PRIVATE
Ws2_32.lib # Winsock libs
)