add_executable(${PROJECT_NAME} 
  main.cpp 
  VirtualDevice.cpp
  VirtualMouse.cpp
  VirtualKeyboard.cpp
  GamepadController.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)