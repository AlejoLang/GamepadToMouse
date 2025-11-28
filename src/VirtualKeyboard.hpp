#ifndef VirtualKeyboard_h
#define VirtualKeyboard_h
#include "VirtualDevice.hpp"
#include <linux/uinput.h>
#include <string.h>
#include <string>

class VirtualKeyboard : public VirtualDevice
{
public:
  struct KeyboardAction : VirtualDevice::Action
  {
    KeyboardAction(int key)
    {
      this->device = VirtualDevice::KEYBOARD;
      this->type = EV_KEY;
      this->key = key;
    }
  };
  inline static KeyboardAction KEY_UP_ACTION = KeyboardAction(KEY_UP);
  inline static KeyboardAction KEY_RIGHT_ACTION = KeyboardAction(KEY_RIGHT);
  inline static KeyboardAction KEY_DOWN_ACTION = KeyboardAction(KEY_DOWN);
  inline static KeyboardAction KEY_LEFT_ACTION = KeyboardAction(KEY_LEFT);

public:
  VirtualKeyboard(std::string name = "Virtual_Keyobard", int vendor_id = 0x1111, int product_id = 0x1112);
  ~VirtualKeyboard() {};
};

#endif