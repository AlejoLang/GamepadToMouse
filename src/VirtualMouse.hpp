#ifndef VirtualMouse_h
#define VirtualMouse_h
#include "VirtualDevice.hpp"
#include <linux/uinput.h>
#include <string>
#include <map>

class VirtualMouse : public VirtualDevice
{
private:
  float sensitivity;

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
  inline static std::map<std::string, VirtualDevice::Action> parser = {
      {"LEFT_CLICK", VirtualMouse::LEFT_CLICK_ACTION},
      {"RIGHT_CLICK", VirtualMouse::RIGHT_CLICK_ACTION},
      {"MIDDLE_CLICK", VirtualMouse::MIDDLE_CLICK_ACTION}};

public:
  VirtualMouse(std::string name = "Default_Name", int vendor_id = 0x1111, int product_id = 0x1111);
  void moveMouseRelativeXY(float dx, float dy);
  void set_sensitivity(float value);
  float get_sensitivity();
  ~VirtualMouse() {};
};

#endif