cmake_minimum_required(VERSION 3.24)
project(Controller_Mouse)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
