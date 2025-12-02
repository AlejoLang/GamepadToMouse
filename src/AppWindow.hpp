#ifndef APPWINDOW_H
#define APPWINDOW_H
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include "ControllersHandler.hpp"

class AppWindow
{
private:
  QApplication *app;
  QMainWindow *main_window;
  QWidget *main_menu;
  ControllersHandler *gamepad_controllers_handler;

public:
  AppWindow(int &argc, char **argv, ControllersHandler *handler);
  void update_menu();
  void run();
  QApplication *get_app();
  QMainWindow *get_main_window();
  ControllersHandler *get_gamepad_controllers_handler();
  ~AppWindow();
};
#endif