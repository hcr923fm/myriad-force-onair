cmake_minimum_required(VERSION 3.7)
set(CMAKE_SYSTEM_VERSION $ENV{WIN_API_VERSION})

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00) # Win10, though there are ways of changing this for the platform
endif()

project(force-myriad-onair CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_GPIO "Compile GPIO triggers for Advantech GPIO cards" OFF)

include_directories("$PROJECT_SOURCE_DIR")

if(ENABLE_GPIO)
    message(STATUS "Compiling Advantech GPIO drivers")
    include_directories(
        "C:/Advantech/DAQNavi/Inc"
    )
    add_definitions(-DMYR_ENABLE_GPIO=1)
endif()

set(Boost_DEBUG ON)
find_package(Boost REQUIRED filesystem system)

include_directories(${Boost_INCLUDE_DIRS})

#if(Boost_ASIO_FOUND)
#    message(STATUS "Found Boost ASIO...")
#    if (Boost_ASIO_LIBRARY)
#        message(STATUS "  Found Boost ASIO libraries")
#    else()
#        message(FATAL_ERROR "  Could not found Boost ASIO library! Exiting...")
#    endif()
#else()
#    message(FATAL_ERROR "Could not find the Boost ASIO component! Exiting...")
#endif()

add_executable(force-myriad-onair
force-onair.cpp
myriad-controller.cpp
gpio-handler.cpp
server.cpp
parse_opts.cpp
)

set(RapidJSON_DIR "C:\\vcpkg\\installed\\x64-windows\\share\\rapidjson")
find_package(RapidJSON CONFIG REQUIRED)
target_include_directories(force-myriad-onair PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(force-myriad-onair PRIVATE
#Ws2_32.lib # Winsock libs
${Boost_LIBRARIES}
${RapidJSON_LIBRARIES}
)