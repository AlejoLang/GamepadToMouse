#include "GamepadConfigPage.hpp"
#include "JoystickSelector.hpp"
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

GamepadConfigPage::GamepadConfigPage(QWidget *parent) : QWidget(parent), m_gamepad_controller(nullptr) {
}

GamepadConfigPage::GamepadConfigPage(GamepadController *controller, QWidget *parent)
    : QWidget(parent), m_gamepad_controller(controller) {
  this->refresh();
}

GamepadController *GamepadConfigPage::get_gamepad_controller() {
  return this->m_gamepad_controller;
}

void GamepadConfigPage::set_gamepad_controller(GamepadController *controller) {
  this->m_gamepad_controller = controller;
}

void GamepadConfigPage::refresh() {
  if (this->m_gamepad_controller == nullptr) {
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

  QVBoxLayout *layout = new QVBoxLayout(this);
  QLabel *title = new QLabel(SDL_GetGamepadName(this->m_gamepad_controller->get_gamepad()), this);
  title->setAlignment(Qt::AlignHCenter);
  layout->addWidget(title);

  QHBoxLayout *slider_layout = new QHBoxLayout();
  QLabel *sens_val = new QLabel(QString::number(this->m_gamepad_controller->get_sensitivity(), 'f', 2), this);
  slider_layout->addWidget(sens_val);
  QSlider *sens_slider = new QSlider(Qt::Horizontal, this);
  sens_slider->setRange(10, 100);
  sens_slider->setTickInterval(10);
  sens_slider->setPageStep(10);
  sens_slider->setValue(this->m_gamepad_controller->get_sensitivity() * 100);
  QObject::connect(sens_slider, &QSlider::valueChanged, [this, sens_val](int val) {
    this->m_gamepad_controller->set_sensitivity(static_cast<float>(val) / 100.f);
    sens_val->setText(QString::number(static_cast<float>(val) / 100.f, 'f', 2));
  });
  slider_layout->addWidget(sens_slider);
  layout->addLayout(slider_layout);

  JoystickSelector *joy_sel = new JoystickSelector(this);
  for (auto gp_joy : this->m_gamepad_controller->get_joystick_parsing_items()) {
    std::string icon_path =
        "./assets/" + (SDL_GetGamepadType(this->m_gamepad_controller->get_gamepad()) == SDL_GAMEPAD_TYPE_STANDARD
                           ? gp_joy.icon_name_xbox
                           : gp_joy.icon_name_ps);
    QIcon *joy_icon = new QIcon(QString::fromStdString(icon_path));
    joy_sel->add_joystick(gp_joy.joystick, joy_icon);
  }
  joy_sel->set_current_joystick(this->m_gamepad_controller->get_mouse_joystick());
  QObject::connect(joy_sel, &JoystickSelector::joystickChanged,
                   [this](GamepadController::Joystick joy) { this->m_gamepad_controller->set_mouse_joystick(joy); });
  layout->addWidget(joy_sel);

  QScrollArea *mouse_buttons_frame = new QScrollArea(this);
  mouse_buttons_frame->setWidgetResizable(true);
  mouse_buttons_frame->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QWidget *mouse_buttons_widget = new QWidget(this);
  QVBoxLayout *mouse_buttons_layout = new QVBoxLayout(mouse_buttons_widget);
  for (auto mouseParser : VirtualMouse::parser) {
    QHBoxLayout *bind_layout = new QHBoxLayout();
    QLabel *bind_name = new QLabel(QString::fromStdString(mouseParser.display_string), mouse_buttons_widget);
    QPushButton *bind_button = new QPushButton(mouse_buttons_widget);
    std::string button_icon_path = this->m_gamepad_controller->get_binded_icon_for_action(mouseParser.action);
    QObject::connect(bind_button, &QPushButton::clicked, [this, mouseParser]() {
      emit this->bindActionButtonPressed(this->m_gamepad_controller->get_gamepad_id(), mouseParser.action);
    });
    bind_button->setIcon(QIcon(QString::fromStdString(button_icon_path)));
    bind_button->setIconSize(QSize(30, 30));
    bind_layout->addWidget(bind_name);
    bind_layout->addStretch(1);
    bind_layout->addWidget(bind_button);
    mouse_buttons_layout->addLayout(bind_layout);
  }
  mouse_buttons_frame->setWidget(mouse_buttons_widget);
  layout->addWidget(mouse_buttons_frame);

  QScrollArea *keyboard_buttons_frame = new QScrollArea(this);
  keyboard_buttons_frame->setWidgetResizable(true);
  keyboard_buttons_frame->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QWidget *keyboard_buttons_widget = new QWidget(this);
  QVBoxLayout *keyboard_buttons_layout = new QVBoxLayout(keyboard_buttons_widget);
  for (auto keyboardParser : VirtualKeyboard::parser) {
    QHBoxLayout *bind_layout = new QHBoxLayout();
    QLabel *bind_name = new QLabel(QString::fromStdString(keyboardParser.display_string), keyboard_buttons_widget);
    QPushButton *bind_button = new QPushButton(keyboard_buttons_widget);
    std::string button_icon_path = this->m_gamepad_controller->get_binded_icon_for_action(keyboardParser.action);
    QObject::connect(bind_button, &QPushButton::clicked, [this, keyboardParser]() {
      emit this->bindActionButtonPressed(this->m_gamepad_controller->get_gamepad_id(), keyboardParser.action);
    });
    bind_button->setIcon(QIcon(QString::fromStdString(button_icon_path)));
    bind_button->setIconSize(QSize(30, 30));
    bind_layout->addWidget(bind_name);
    bind_layout->addStretch(1);
    bind_layout->addWidget(bind_button);
    keyboard_buttons_layout->addLayout(bind_layout);
  }
  keyboard_buttons_frame->setWidget(keyboard_buttons_widget);
  layout->addWidget(keyboard_buttons_frame);

  layout->addStretch(1);
  QPushButton *back_btn = new QPushButton("Back to menu", this);
  QObject::connect(back_btn, &QPushButton::clicked, [this]() { emit this->backPressed(); });
  layout->addWidget(back_btn);
}