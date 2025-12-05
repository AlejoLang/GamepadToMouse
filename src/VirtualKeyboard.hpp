#ifndef VirtualKeyboard_h
#define VirtualKeyboard_h
#include "VirtualDevice.hpp"
#include <linux/uinput.h>
#include <map>
#include <string>

class VirtualKeyboard : public VirtualDevice {
public:
  struct KeyboardAction : VirtualDevice::Action {
    KeyboardAction(int key) {
      this->device = VirtualDevice::KEYBOARD;
      this->type = EV_KEY;
      this->key = key;
    }
  };
  inline static KeyboardAction KEY_UP_ACTION = KeyboardAction(KEY_UP);
  inline static KeyboardAction KEY_RIGHT_ACTION = KeyboardAction(KEY_RIGHT);
  inline static KeyboardAction KEY_DOWN_ACTION = KeyboardAction(KEY_DOWN);
  inline static KeyboardAction KEY_LEFT_ACTION = KeyboardAction(KEY_LEFT);
  inline static std::map<std::string, VirtualDevice::Action> parser = {{"KEY_UP", VirtualKeyboard::KEY_UP_ACTION},
                                                                       {"KEY_RIGHT", VirtualKeyboard::KEY_RIGHT_ACTION},
                                                                       {"KEY_DOWN", VirtualKeyboard::KEY_DOWN_ACTION},
                                                                       {"KEY_LEFT", VirtualKeyboard::KEY_LEFT_ACTION}};

public:
  VirtualKeyboard(std::string name = "Virtual_Keyobard", int vendor_id = 0x1111, int product_id = 0x1112);
  ~VirtualKeyboard() {};
};

#endif