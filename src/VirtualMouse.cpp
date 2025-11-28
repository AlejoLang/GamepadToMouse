#include "VirtualMouse.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

VirtualMouse::VirtualMouse(std::string name, int vendor_id, int product_id) : VirtualDevice("/dev/uinput")
{
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
  strcpy(usetup.name, name.c_str());

  ioctl(this->device_file, UI_DEV_SETUP, &usetup);
  ioctl(this->device_file, UI_DEV_CREATE);
}

void VirtualMouse::moveMouseRelativeXY(int dx, int dy)
{
  if (!dx && !dy)
  {
    return;
  }
  this->emit(EV_REL, REL_X, dx);
  this->emit(EV_REL, REL_Y, dy);
  this->send_sync_report();
}