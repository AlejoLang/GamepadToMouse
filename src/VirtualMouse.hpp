#ifndef VirtualMouse_h
#define VirtualMouse_h
#include "VirtualDevice.hpp"
#include <linux/uinput.h>
#include <string>
#include <vector>

class VirtualMouse : public VirtualDevice {
private:
  float sensitivity;

public:
  struct MouseKeyAction : VirtualDevice::Action {
    MouseKeyAction(int key) {
      this->device = VirtualDevice::MOUSE;
      this->type = EV_KEY;
      this->key = key;
    }
  };
  inline static MouseKeyAction LEFT_CLICK_ACTION = MouseKeyAction(BTN_LEFT);
  inline static MouseKeyAction RIGHT_CLICK_ACTION = MouseKeyAction(BTN_RIGHT);
  inline static MouseKeyAction MIDDLE_CLICK_ACTION = MouseKeyAction(BTN_MIDDLE);
  inline static std::vector<VirtualDevice::ParsingItem> parser = {
      {VirtualMouse::LEFT_CLICK_ACTION, "LEFT_CLICK", "Mouse Left Click"},
      {VirtualMouse::RIGHT_CLICK_ACTION, "RIGHT_CLICK", "Mouse Right Click"},
      {VirtualMouse::MIDDLE_CLICK_ACTION, "MIDDLE_CLICK", "Mouse Middle Click"}};

public:
  VirtualMouse(std::string name = "Default_Name", int vendor_id = 0x1111, int product_id = 0x1111);
  void moveMouseRelativeXY(float dx, float dy);
  void set_sensitivity(float value);
  float get_sensitivity();
  ~VirtualMouse() {};
};

#endif