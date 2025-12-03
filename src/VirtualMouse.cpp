#include "VirtualMouse.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cmath>

VirtualMouse::VirtualMouse(std::string name, int vendor_id, int product_id)
{
  this->sensitivity = 0.2;
  struct uinput_setup usetup;

  /* enable mouse button left and relative events */
  ioctl(this->device_file, UI_SET_EVBIT, EV_KEY);
  ioctl(this->device_file, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(this->device_file, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(this->device_file, UI_SET_KEYBIT, BTN_MIDDLE);

  ioctl(this->device_file, UI_SET_EVBIT, EV_REL);
  ioctl(this->device_file, UI_SET_RELBIT, REL_X);
  ioctl(this->device_file, UI_SET_RELBIT, REL_Y);

  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = vendor_id;
  usetup.id.product = product_id;
  strncpy(usetup.name, name.c_str(), sizeof(usetup.name));
  usetup.name[sizeof(usetup.name) - 1] = '\0';

  ioctl(this->device_file, UI_DEV_SETUP, &usetup);
  ioctl(this->device_file, UI_DEV_CREATE);
}

void VirtualMouse::moveMouseRelativeXY(float dx, float dy)
{
  if (!dx && !dy)
  {
    return;
  }
  this->emit_event(EV_REL, REL_X, 50 * dx * this->sensitivity);
  this->emit_event(EV_REL, REL_Y, 50 * dy * this->sensitivity);
  this->send_sync_report();
}

void VirtualMouse::set_sensitivity(float value)
{
  // Clamp value to [0.0, 1.0]
  this->sensitivity = std::max(0.0f, std::min(value, 1.0f));
}

float VirtualMouse::get_sensitivity()
{
  return this->sensitivity;
}