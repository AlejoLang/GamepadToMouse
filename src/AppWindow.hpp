#ifndef APPWINDOW_H
#define APPWINDOW_H
#include "ControllersHandler.hpp"
#include "ui/ControllersListPage.hpp"
#include "ui/GamepadConfigPage.hpp"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStackedWidget>

class AppWindow {
private:
  QApplication *app;
  QMainWindow *main_window;
  ControllersListPage *controllers_list;
  GamepadConfigPage *config_page;
  QStackedWidget *stack;
  ControllersHandler *gamepad_controllers_handler;
  bool gamepad_input; // Set to true to request input from the curren gamepad to bind a button
  VirtualDevice::Action to_bind_action;
  SDL_JoystickID to_bind_gamepad_id;

public:
  AppWindow(int &argc, char **argv, ControllersHandler *handler);
  void update_menu();
  void run();
  void create_config_page(GamepadController *controller);
  void delete_config_page();
  QApplication *get_app();
  QMainWindow *get_main_window();
  ControllersHandler *get_gamepad_controllers_handler();
  QWidget *get_controllers_list();
  QStackedWidget *get_stack();
  bool wants_gamepad_input();
  void set_gamepad_input(SDL_JoystickID id, SDL_GamepadButton btn);
  ~AppWindow();
};
#endif