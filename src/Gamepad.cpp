#include "Gamepad.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/joystick.h>
#include <string>

Gamepad::Gamepad()
{
  this->gamepad_file = open("/dev/input/js0", O_NONBLOCK);
}

js_event Gamepad::get_event()
{
  js_event event;
  int resp = read(this->gamepad_file, &event, sizeof(event));
  if (resp == -1)
  {
    event.type = -1;
  }
  return event;
}

Gamepad::~Gamepad()
{
  close(this->gamepad_file);
}