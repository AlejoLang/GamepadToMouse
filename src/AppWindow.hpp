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

public:
  AppWindow(int &argc, char **argv, ControllersHandler *handler);
  void update_menu();
  void run();
  QApplication *get_app();
  QMainWindow *get_main_window();
  ControllersHandler *get_gamepad_controllers_handler();
  QWidget *get_controllers_list();
  QStackedWidget *get_stack();
  ~AppWindow();
};
#endif