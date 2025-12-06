#include "ControllersListPage.hpp"
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>

ControllersListPage::ControllersListPage(QWidget *parent) : QWidget(parent) {
  this->m_controllers_handler = nullptr;
}

ControllersListPage::ControllersListPage(ControllersHandler *handler, QWidget *parent) : QWidget(parent) {
  this->m_controllers_handler = handler;
  this->refresh();
}

void ControllersListPage::set_handler(ControllersHandler *handler) {
  this->m_controllers_handler = handler;
  this->refresh();
}

ControllersHandler *ControllersListPage::get_handler() {
  return this->m_controllers_handler;
}

void ControllersListPage::refresh() {
  if (this->m_controllers_handler == nullptr) {
    return;
  }
  if (this->layout()) {
    QLayoutItem *item;
    while ((item = this->layout()->takeAt(0)) != nullptr) {
      delete item->widget();
      delete item;
    }
    delete this->layout();
  }

  QVBoxLayout *main_layout = new QVBoxLayout(this);

  QScrollArea *scroll_area = new QScrollArea(this);
  scroll_area->setWidgetResizable(true);
  scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QWidget *gamepads_list = new QWidget();
  QVBoxLayout *content_layout = new QVBoxLayout(gamepads_list);

  if (this->m_controllers_handler->get_controllers().empty()) {
    QLabel *no_gp_label = new QLabel("No gamepad detected", gamepads_list);
    no_gp_label->setAlignment(Qt::AlignCenter);
    content_layout->addWidget(no_gp_label);
  } else {
    for (auto *controller : this->m_controllers_handler->get_controllers()) {
      QPushButton *new_gamepad_btn = new QPushButton(SDL_GetGamepadName(controller->get_gamepad()), gamepads_list);
      QObject::connect(new_gamepad_btn, &QPushButton::clicked,
                       [this, controller]() { emit this->controllerSelected(controller); });
      content_layout->addWidget(new_gamepad_btn);
    }
    content_layout->addStretch(1);
  }

  scroll_area->setWidget(gamepads_list);
  main_layout->addWidget(scroll_area);
}