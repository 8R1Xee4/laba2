cmake_minimum_required(VERSION 3.5...4.0)
project(laba2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt6 COMPONENTS Widgets GUI Core REQUIRED)

file(GLOB CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB UI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

add_executable(laba2
    WIN32
    ${CPP_SOURCES}
    ${UI_FILES}
)

target_link_libraries(laba2
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
)