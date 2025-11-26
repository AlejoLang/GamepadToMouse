add_executable(${PROJECT_NAME} 
  main.cpp 
  VirtualMouse.cpp
  Gamepad.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)