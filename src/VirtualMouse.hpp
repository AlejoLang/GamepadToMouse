#ifndef VirtualMouse_h
#define VirtualMouse_h
#include "VirtualDevice.hpp"
#include <linux/uinput.h>
#include <string>

class VirtualMouse : public VirtualDevice
{
public:
  struct MouseKeyAction : VirtualDevice::Action
  {
    MouseKeyAction(int key)
    {
      this->device = VirtualDevice::MOUSE;
      this->type = EV_KEY;
      this->key = key;
    }
  };
  inline static MouseKeyAction LEFT_CLICK_ACTION = MouseKeyAction(BTN_LEFT);
  inline static MouseKeyAction RIGHT_CLICK_ACTION = MouseKeyAction(BTN_RIGHT);
  inline static MouseKeyAction MIDDLE_CLICK_ACTION = MouseKeyAction(BTN_MIDDLE);

public:
  VirtualMouse(std::string name = "Default_Name", int vendor_id = 0x1111, int product_id = 0x1111);
  void moveMouseRelativeXY(int dx, int dy);
  ~VirtualMouse() {};
};

#endif