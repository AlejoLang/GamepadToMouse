#ifndef GamepadConfigPage_H
#define GamepadConfigPage_H
#include "../GamepadController.hpp"
#include <QtWidgets/QWidget>

class GamepadConfigPage : public QWidget {
  Q_OBJECT
  Q_PROPERTY(GamepadController *gamepad_controller READ get_gamepad_controller WRITE set_gamepad_controller)

private:
  GamepadController *m_gamepad_controller;

public:
  GamepadConfigPage(QWidget *parent = nullptr);
  GamepadConfigPage(GamepadController *controller, QWidget *parent = nullptr);
  void set_gamepad_controller(GamepadController *controller);
  GamepadController *get_gamepad_controller();
  void refresh();

signals:
  void backPressed();
  void bindActionButtonPressed(SDL_JoystickID id, VirtualDevice::Action act);
};

#endif