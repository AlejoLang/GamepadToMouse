#ifndef JoystickSelector_H
#define JoystickSelector_H
#include "../GamepadController.hpp"
#include <QIcon>
#include <QLabel>
#include <QWidget>
#include <vector>

class JoystickSelector : public QWidget {
  Q_OBJECT
  Q_PROPERTY(GamepadController::Joystick joystick);
  Q_PROPERTY(int selected_item_idx)
public:
  struct Item {
    GamepadController::Joystick joystick;
    QIcon *icon;
  };

private:
  Item m_selected_joystick;
  int m_selected_joystick_idx;
  std::vector<Item> m_joysticks;
  QLabel *m_icon_label = nullptr;

public:
  JoystickSelector(QWidget *parent = nullptr);
  JoystickSelector(std::vector<Item> joysticks, QWidget *parent = nullptr);
  void add_joystick(GamepadController::Joystick new_joystick, QIcon *icon);
  void set_current_joystick(GamepadController::Joystick joy);
  void refresh();

private:
  void updateIcon();
signals:
  void joystickChanged(GamepadController::Joystick);
};

#endif