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
  this->to_bind_action = {};
  this->to_bind_gamepad_id = -1;
  this->gamepad_input = false;
  this->main_window = new QMainWindow();
  this->main_window->setWindowTitle("GamepadInput");
  this->main_window->resize(QSize(400, 600));
  this->main_window->setContentsMargins(QMargins(20, 20, 20, 20));
  this->stack = new QStackedWidget(this->main_window);

  this->controllers_list = new ControllersListPage(handler, this->main_window);
  QObject::connect(this->controllers_list, &ControllersListPage::controllerSelected,
                   [this](GamepadController *controller) { this->create_config_page(controller); });

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

void AppWindow::create_config_page(GamepadController *controller) {
  this->config_page = new GamepadConfigPage(controller, this->main_window);
  this->stack->addWidget(this->config_page);
  this->stack->setCurrentWidget(this->config_page);
  QObject::connect(this->config_page, &GamepadConfigPage::backPressed, [this]() { this->delete_config_page(); });
  QObject::connect(this->config_page, &GamepadConfigPage::bindActionButtonPressed,
                   [this](SDL_JoystickID id, VirtualDevice::Action act) {
                     this->gamepad_input = true;
                     this->to_bind_action = act;
                     this->to_bind_gamepad_id = id;
                   });
}

void AppWindow::delete_config_page() {
  this->stack->removeWidget(this->config_page);
  this->stack->setCurrentWidget(this->controllers_list);
  this->config_page->deleteLater();
  this->config_page = nullptr;
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

bool AppWindow::wants_gamepad_input() {
  return this->gamepad_input;
}

void AppWindow::set_gamepad_input(SDL_JoystickID id, SDL_GamepadButton btn) {
  if (id != this->to_bind_gamepad_id || this->to_bind_gamepad_id == -1 ||
      this->to_bind_action == VirtualDevice::Action()) {
    return;
  }
  this->gamepad_controllers_handler->get_current_controller()->set_keybind(btn, this->to_bind_action);
  this->to_bind_action = {};
  this->to_bind_gamepad_id = -1;
  this->gamepad_input = false;
  this->config_page->refresh();
}

AppWindow::~AppWindow() {
  delete this->main_window;
  delete this->app;
}