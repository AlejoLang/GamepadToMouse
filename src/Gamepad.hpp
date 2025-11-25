#include <linux/joystick.h>

struct Joystick
{
  int16_t x;
  int16_t y;
};

class Gamepad
{
private:
  int gamepad_file;

public:
  Joystick joy1;
  Joystick joy2;
  Joystick dpad;
  int16_t left_trigger;
  int16_t right_trigger;

public:
  static const int EV_JOYSTICK = JS_EVENT_AXIS;
  static const int EV_BUTTON = JS_EVENT_BUTTON;
  static const int MAX_ANALOG_VALUE = 32767;
  int JOY1_X = 0; // Left Joystick X axis
  int JOY1_Y = 1; // Left Joystick Y axis
  int JOY2_X = 3; // Right Joystick X axis
  int JOY2_Y = 4; // Tighe Joystick Y axis
  int DPAD_X = 6; // Dpad X axis
  int DPAD_Y = 7; // Dpad Y axis
  int TRGG_L = 2; // Left Trigger Analog
  int TRGG_R = 5; // Right Trigger Analog

  int BTN_CR = 0;  // Cross
  int BTN_CI = 1;  // Circle
  int BTN_TI = 2;  // Triangle
  int BTN_SQ = 3;  // Square
  int BTN_LB = 4;  // Left Button
  int BTN_RB = 5;  // Right Button
  int BTN_LT = 6;  // Left Trigger Button (When fully pressed or released)
  int BTN_RT = 7;  // Right Trigger Button (When fully pressed or released)
  int BTN_SH = 8;  // Share Button
  int BTN_OP = 9;  // Options Button
  int BTN_ST = 10; // Power / Main Button
  int BTN_J1 = 11; // Left Joystick Button
  int BTN_J2 = 12; // Right Joytsick Button

public:
  Gamepad();
  js_event get_event();
  ~Gamepad();
};