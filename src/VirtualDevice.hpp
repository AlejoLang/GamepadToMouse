#ifndef VirtualDevice_h
#define VirtualDevice_h
#include <string>

class VirtualDevice
{
protected:
  int device_file;

public:
  enum DeviceType
  {
    MOUSE,
    KEYBOARD
  };
  struct Action
  {
    DeviceType device;
    int type;
    int key;
  };

public:
  VirtualDevice(std::string device_path);
  int get_file();
  void do_key_action(Action action, int value);
  ~VirtualDevice();

protected:
  void emit(int type, int key, int value);
  void send_sync_report();
};

#endif