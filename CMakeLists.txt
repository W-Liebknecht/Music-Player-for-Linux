cmake_minimum_required(VERSION 3.10)
project(MP3Player LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 开启 Qt 自动化编译工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 2. 查找依赖包
# A. 查找 SFML
find_package(SFML 2.5 COMPONENTS audio REQUIRED)

# B. 查找 Qt (优先 Qt6，兼容 Qt5)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# C. 查找 TagLib (使用 PkgConfig 避开配置文件缺失报错)
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)

# 3. 定义可执行文件目标 (必须先定义 add_executable，后面才能配置 target_xxx)
add_executable(MusikSpieler
    src/main.cpp
    src/Player.cpp
    src/SignalInput.cpp
    src/DesktopLyrics.cpp
    include/DesktopLyrics.hpp
    src/LyricParser.cpp 
    include/LyricParser.hpp 
)

# 4. 设置头文件搜索目录
target_include_directories(MusikSpieler PRIVATE
    include/
    ${TAGLIB_INCLUDE_DIRS}
)

# 5. 统一链接所有依赖库 (Qt + SFML + TagLib)
target_link_libraries(MusikSpieler PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    sfml-audio
    ${TAGLIB_LIBRARIES}
)
