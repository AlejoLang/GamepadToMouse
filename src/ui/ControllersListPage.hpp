#ifndef ControllersListPage_H
#define ControllersListPage_H
#include "../ControllersHandler.hpp"
#include <QtWidgets/QWidget>

class ControllersListPage : public QWidget {
  Q_OBJECT
  Q_PROPERTY(ControllersHandler *controllers_handler READ get_handler WRITE set_handler);

private:
  ControllersHandler *m_controllers_handler;

public:
  ControllersListPage(QWidget *parent = nullptr);
  ControllersListPage(ControllersHandler *handler, QWidget *parent = nullptr);
  void refresh();
  void set_handler(ControllersHandler *handler);
  ControllersHandler *get_handler();
signals:
  void controllerSelected(GamepadController *controller);
};
#endif