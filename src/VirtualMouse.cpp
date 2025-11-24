#include "VirtualMouse.hpp"

VirtualMouse::VirtualMouse(std::string name, int vendor_id, int product_id)
{
  struct uinput_setup usetup;

  this->mouse_file = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  /* enable mouse button left and relative events */
  ioctl(this->mouse_file, UI_SET_EVBIT, EV_KEY);
  ioctl(this->mouse_file, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(this->mouse_file, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(this->mouse_file, UI_SET_KEYBIT, BTN_MIDDLE);

  ioctl(this->mouse_file, UI_SET_EVBIT, EV_REL);
  ioctl(this->mouse_file, UI_SET_RELBIT, REL_X);
  ioctl(this->mouse_file, UI_SET_RELBIT, REL_Y);

  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = vendor_id;
  usetup.id.product = product_id;
  strcpy(usetup.name, name.c_str());

  ioctl(this->mouse_file, UI_DEV_SETUP, &usetup);
  ioctl(this->mouse_file, UI_DEV_CREATE);
}

void VirtualMouse::emit(int type, int code, int val)
{
  input_event new_event;

  new_event.type = type;
  new_event.code = code;
  new_event.value = val;
  new_event.time.tv_sec = 0;
  new_event.time.tv_usec = 0;

  write(this->mouse_file, &new_event, sizeof(new_event));
}

void VirtualMouse::moveMouseRelative(int axis, int pixels)
{
  this->emit(EV_REL, axis, pixels);
  send_sync_report();
}

void VirtualMouse::pressButton(int button)
{
  this->emit(EV_KEY, button, 1);
  send_sync_report();
}

void VirtualMouse::releaseButton(int button)
{
  this->emit(EV_KEY, button, 0);
  send_sync_report();
}

void VirtualMouse::send_sync_report()
{
  this->emit(EV_SYN, SYN_REPORT, 0);
  usleep(10000);
}

VirtualMouse::~VirtualMouse()
{
  ioctl(this->mouse_file, UI_DEV_DESTROY);
  close(this->mouse_file);
}