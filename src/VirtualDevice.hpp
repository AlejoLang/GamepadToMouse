#ifndef VirtualDevice_h
#define VirtualDevice_h

#define UINPUT_DEVICE_PATH "/dev/uinput"

class VirtualDevice {
protected:
  int device_file;

public:
  enum DeviceType {
    MOUSE,
    KEYBOARD
  };
  struct Action {
    DeviceType device;
    int type;
    int key;
    bool operator==(Action const &a) const {
      return (this->device == a.device && this->type == a.type && this->key == a.key);
    }
    bool operator!=(Action const &a) const {
      return !(*this == a);
    }
  };

public:
  VirtualDevice();
  int get_file();
  void do_key_action(Action action, int value);
  ~VirtualDevice();

protected:
  void emit_event(int type, int key, int value);
  void send_sync_report();
};

#endif