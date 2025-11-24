cmake_minimum_required(VERSION 3.24)
project(Controller_Mouse)

set(CMAKE_CXX_STANDAR 23)
set(CMAKE_CXX_STANDAR_REQUIERED ON)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
  main.cpp
)

