#include "VirtualKeyboard.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

VirtualKeyboard::VirtualKeyboard(std::string name, int vendor_id, int product_id) : VirtualDevice("/dev/uinput")
{
  struct uinput_setup usetup;

  /* enable keyboard events */
  ioctl(this->device_file, UI_SET_EVBIT, EV_KEY);

  /* enable all keyboard keys */
  for (int i = KEY_ESC; i < KEY_MAX; i++)
  {
    ioctl(this->device_file, UI_SET_KEYBIT, i);
  }

  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = vendor_id;
  usetup.id.product = product_id;
  strcpy(usetup.name, name.c_str());

  ioctl(this->device_file, UI_DEV_SETUP, &usetup);
  ioctl(this->device_file, UI_DEV_CREATE);
}