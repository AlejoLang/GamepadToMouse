#include "AppWindow.hpp"
#include "GamepadController.hpp"
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>

AppWindow::AppWindow(int &argc, char **argv, ControllersHandler *handler) {
  this->app = new QApplication(argc, argv);
  this->gamepad_controllers_handler = handler;
  this->main_window = new QMainWindow();
  this->main_window->setWindowTitle("GamepadInput");
  this->main_window->resize(QSize(400, 600));
  this->main_window->setContentsMargins(QMargins(20, 20, 20, 20));
  this->stack = new QStackedWidget(this->main_window);

  this->controllers_list = new ControllersListPage(handler, this->main_window);
  QObject::connect(this->controllers_list, &ControllersListPage::controllerSelected,
                   [this](GamepadController *controller) {
                     this->config_page = new GamepadConfigPage(controller, this->main_window);
                     this->stack->addWidget(this->config_page);
                     this->stack->setCurrentWidget(this->config_page);
                     QObject::connect(this->config_page, &GamepadConfigPage::backPressed, [this]() {
                       this->stack->removeWidget(this->config_page);
                       this->stack->setCurrentWidget(this->controllers_list);
                     });
                   });

  this->stack->addWidget(this->controllers_list);
  this->stack->setCurrentWidget(this->controllers_list);
  this->main_window->setCentralWidget(this->stack);
  this->main_window->show();
}

void AppWindow::update_menu() {
  this->controllers_list->refresh();
}

void AppWindow::run() {
  this->app->exec();
}

QApplication *AppWindow::get_app() {
  return this->app;
}

QMainWindow *AppWindow::get_main_window() {
  return this->main_window;
}

ControllersHandler *AppWindow::get_gamepad_controllers_handler() {
  return this->gamepad_controllers_handler;
}

QWidget *AppWindow::get_controllers_list() {
  return this->controllers_list;
}

QStackedWidget *AppWindow::get_stack() {
  return this->stack;
}

AppWindow::~AppWindow() {
  delete this->main_window;
  delete this->app;
}