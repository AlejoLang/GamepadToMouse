#include <linux/uinput.h>
#include <string.h>
#include <string>

#define sci static const int

class VirtualMouse
{
private:
  int mouse_file;

public:
  sci Y_AXIS = REL_Y;
  sci X_AXIS = REL_X;
  sci LEFT_CLICK = BTN_LEFT;
  sci RIGHT_CLICK = BTN_RIGHT;
  sci MIDDLE_CLICK = BTN_MIDDLE;
  sci SCROLL_UP = KEY_SCROLLUP;
  sci SCROLL_DOWN = KEY_SCROLLDOWN;

private:
  void send_sync_report();
  void emit(int type, int code, int val);

public:
  VirtualMouse(std::string name = "Default_Name", int vendor_id = 0x1111, int product_id = 0x1111);
  void moveMouseRelativeXY(int dx, int dy);
  void pressButton(int button);
  void releaseButton(int button);
  ~VirtualMouse();
};