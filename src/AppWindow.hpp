#ifndef APPWINDOW_H
#define APPWINDOW_H
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStackedWidget>
#include "ControllersHandler.hpp"

class AppWindow
{
private:
  QApplication *app;
  QMainWindow *main_window;
  QWidget *main_menu;
  QWidget *current_config_menu;
  QStackedWidget *stack;
  ControllersHandler *gamepad_controllers_handler;

public:
  AppWindow(int &argc, char **argv, ControllersHandler *handler);
  void update_menu();
  void run();
  QApplication *get_app();
  QMainWindow *get_main_window();
  ControllersHandler *get_gamepad_controllers_handler();
  QWidget *get_main_menu();
  QStackedWidget *get_stack();
  QWidget *get_current_config_menu();
  void set_current_config_menu(QWidget *w);
  ~AppWindow();
};
#endif