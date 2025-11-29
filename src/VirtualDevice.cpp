#include "VirtualDevice.hpp"
#include <linux/uinput.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

VirtualDevice::VirtualDevice()
{
  this->device_file = open(UINPUT_DEVICE_PATH, O_WRONLY | O_NONBLOCK);
}

int VirtualDevice::get_file()
{
  return this->device_file;
}

void VirtualDevice::do_key_action(Action action, int value)
{
  this->emit(action.type, action.key, value);
  this->send_sync_report();
}

void VirtualDevice::emit(int type, int key, int value)
{
  input_event new_event;

  new_event.type = type;
  new_event.code = key;
  new_event.value = value;
  new_event.time.tv_sec = 0;
  new_event.time.tv_usec = 0;

  write(this->device_file, &new_event, sizeof(new_event));
}

void VirtualDevice::send_sync_report()
{
  this->emit(EV_SYN, SYN_REPORT, 0);
  usleep(10000);
}

VirtualDevice::~VirtualDevice()
{
  ioctl(this->device_file, UI_DEV_DESTROY);
  close(this->device_file);
}