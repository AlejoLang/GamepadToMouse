#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/uinput.h>
#include <linux/joystick.h>

int setup_virtual_mouse();
void emit(int fd, int type, int code, int val);
void process_event(js_event event);

int16_t joy_x = 0;
int16_t joy_y = 0;
bool exit_program = false;

int mouse_file;
int gamepad_file;

int main()
{
  mouse_file = setup_virtual_mouse();
  gamepad_file = open("/dev/input/js0", O_NONBLOCK);

  if (!mouse_file)
  {
    std::cout << "Error creating virtual mouse" << std::endl;
    return -1;
  }
  if (!gamepad_file)
  {
    std::cout << "Error opening gamepad" << std::endl;
    return -1;
  }

  js_event event;
  while (!exit_program)
  {
    while (read(gamepad_file, &event, sizeof(event)) > 0)
    {
      process_event(event);
    }
    emit(mouse_file, EV_REL, REL_X, 10 * ((float)joy_x / 32767));
    emit(mouse_file, EV_REL, REL_Y, 10 * ((float)joy_y / 32767));
    emit(mouse_file, EV_SYN, SYN_REPORT, 0);
    usleep(10000);
  }
  sleep(1);
  ioctl(mouse_file, UI_DEV_DESTROY);
  close(mouse_file);
  close(gamepad_file);
  return 0;
}

int setup_virtual_mouse()
{
  struct uinput_setup usetup;

  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  /* enable mouse button left and relative events */
  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);

  ioctl(fd, UI_SET_EVBIT, EV_REL);
  ioctl(fd, UI_SET_RELBIT, REL_X);
  ioctl(fd, UI_SET_RELBIT, REL_Y);

  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = 0x1111;  /* sample vendor */
  usetup.id.product = 0x1111; /* sample product */
  strcpy(usetup.name, "Gamepad_Virtual");

  ioctl(fd, UI_DEV_SETUP, &usetup);
  ioctl(fd, UI_DEV_CREATE);

  return fd;
}

void emit(int fd, int type, int code, int val)
{
  struct input_event ie;

  ie.type = type;
  ie.code = code;
  ie.value = val;
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;

  write(fd, &ie, sizeof(ie));
}

void process_event(js_event event)
{
  switch (event.type)
  {
  case JS_EVENT_AXIS:
    if (event.number == 0)
    {
      joy_x = event.value;
    }
    else if (event.number == 1)
    {
      joy_y = event.value;
    }
    std::cout << joy_x << ", " << joy_y << std::endl;
    break;
  case JS_EVENT_BUTTON:
    std::cout << (int)event.number << std::endl;
    if (event.number == 10)
    {
      exit_program = true;
    }
    if (event.number == 0)
    {
      emit(mouse_file, EV_KEY, BTN_LEFT, event.value);
      emit(mouse_file, EV_SYN, SYN_REPORT, 0);
    }
    break;
  }
}