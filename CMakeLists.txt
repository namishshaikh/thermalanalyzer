cmake_minimum_required(VERSION 3.5)

project(ThermalAnalyzer)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/Headers/src
    ${OpenCV_INCLUDE_DIRS}
)

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS Headers/src/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    ${OpenCV_LIBS}
    Qt5::Network
)


