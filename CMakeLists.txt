cmake_minimum_required(VERSION 3.10)
project(MP3Player)

set(CMAKE_CXX_STANDARD 17)
include_directories(include/)
find_package(SFML 2.5 COMPONENTS audio REQUIRED)

add_executable(MusikSpieler
    src/main.cpp
    src/Player.cpp
    src/SignalInput.cpp
)

target_link_libraries(MusikSpieler sfml-audio)
