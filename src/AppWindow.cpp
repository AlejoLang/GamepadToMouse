#include "AppWindow.hpp"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QStackedWidget>
#include "GamepadController.hpp"

static QWidget *create_controller_config_menu(AppWindow *app_window, GamepadController *controller);
static QWidget *create_main_menu(AppWindow *app_window);

static QWidget *create_controller_config_menu(AppWindow *app_window, GamepadController *controller)
{
  QWidget *controller_config_widget = new QWidget();
  QVBoxLayout *config_layout = new QVBoxLayout(controller_config_widget);
  QLabel *label = new QLabel(SDL_GetGamepadName(controller->get_gamepad()), controller_config_widget);
  config_layout->addWidget(label);
  QPushButton *btn = new QPushButton("Back", controller_config_widget);
  QObject::connect(btn, &QPushButton::clicked, [app_window]()
                   { app_window->get_stack()->removeWidget(app_window->get_current_config_menu());
                      app_window->get_stack()->setCurrentWidget(app_window->get_main_menu()); });
  config_layout->addStretch(1);
  config_layout->addWidget(btn);
  return controller_config_widget;
}

static QWidget *create_main_menu(AppWindow *app_window)
{
  QWidget *menu_widget = new QWidget(app_window->get_main_window());
  QVBoxLayout *menu_layout = new QVBoxLayout(menu_widget);
  if (app_window->get_gamepad_controllers_handler()->get_controllers().empty())
  {
    QLabel *no_gp_label = new QLabel("No gamepad detected", menu_widget);
    no_gp_label->setAlignment(Qt::AlignHCenter);
    menu_layout->addWidget(no_gp_label);
  }
  else
  {
    for (GamepadController *controller : app_window->get_gamepad_controllers_handler()->get_controllers())
    {
      QPushButton *btn = new QPushButton(QString::fromStdString(SDL_GetGamepadName(controller->get_gamepad())), menu_widget);
      QObject::connect(btn, &QPushButton::clicked, [app_window, controller]()
                       {
        app_window->set_current_config_menu(create_controller_config_menu(app_window, controller));
        app_window->get_stack()->addWidget(app_window->get_current_config_menu());
      app_window->get_stack()->setCurrentWidget(app_window->get_current_config_menu()); });
      menu_layout->addWidget(btn);
    }
    menu_layout->addStretch(1);
  }
  return menu_widget;
}

AppWindow::AppWindow(int &argc, char **argv, ControllersHandler *handler)
{
  this->app = new QApplication(argc, argv);
  this->gamepad_controllers_handler = handler;
  this->main_window = new QMainWindow();
  this->main_window->setWindowTitle("GamepadInput");
  this->main_window->resize(QSize(400, 600));
  this->main_window->setContentsMargins(QMargins(20, 20, 20, 20));
  this->stack = new QStackedWidget(this->main_window);

  this->main_menu = create_main_menu(this);
  this->stack->addWidget(this->main_menu);
  this->stack->setCurrentWidget(this->main_menu);
  this->main_window->setCentralWidget(this->stack);
  this->main_window->show();
}

void AppWindow::update_menu()
{
  this->stack->removeWidget(this->main_menu);
  this->main_menu = create_main_menu(this);
  this->stack->addWidget(this->main_menu);
  this->stack->setCurrentWidget(this->main_menu);
}

void AppWindow::run()
{
  this->app->exec();
}

QApplication *AppWindow::get_app()
{
  return this->app;
}

QMainWindow *AppWindow::get_main_window()
{
  return this->main_window;
}

ControllersHandler *AppWindow::get_gamepad_controllers_handler()
{
  return this->gamepad_controllers_handler;
}

QWidget *AppWindow::get_main_menu()
{
  return this->main_menu;
}

QStackedWidget *AppWindow::get_stack()
{
  return this->stack;
}

QWidget *AppWindow::get_current_config_menu()
{
  return this->current_config_menu;
}

void AppWindow::set_current_config_menu(QWidget *w)
{
  this->current_config_menu = w;
}

AppWindow::~AppWindow()
{
  delete this->main_window;
  delete this->app;
}