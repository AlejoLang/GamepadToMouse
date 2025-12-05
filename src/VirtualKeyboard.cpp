#include "VirtualKeyboard.hpp"
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

VirtualKeyboard::VirtualKeyboard(std::string name, int vendor_id, int product_id) {
  struct uinput_setup usetup;

  /* enable keyboard events */
  ioctl(this->device_file, UI_SET_EVBIT, EV_KEY);

  /* enable all keyboard keys */
  for (int i = KEY_ESC; i < KEY_MAX; i++) {
    ioctl(this->device_file, UI_SET_KEYBIT, i);
  }

  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = vendor_id;
  usetup.id.product = product_id;
  strncpy(usetup.name, name.c_str(), sizeof(usetup.name));
  usetup.name[sizeof(usetup.name) - 1] = '\0';

  ioctl(this->device_file, UI_DEV_SETUP, &usetup);
  ioctl(this->device_file, UI_DEV_CREATE);
}