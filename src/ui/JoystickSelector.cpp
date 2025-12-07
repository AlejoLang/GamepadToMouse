#include "JoystickSelector.hpp"
#include <QHBoxLayout>
#include <QPushButton>

JoystickSelector::JoystickSelector(QWidget *parent) : QWidget(parent) {
  this->m_selected_joystick_idx = 0;
  this->m_joysticks = {};
  this->m_selected_joystick = {};
}

JoystickSelector::JoystickSelector(std::vector<Item> joysticks, QWidget *parent) : QWidget(parent) {
  this->m_joysticks = joysticks;
  this->m_selected_joystick_idx = 0;
  this->m_selected_joystick = m_joysticks[0];
  emit this->joystickChanged(this->m_selected_joystick.joystick);
  this->refresh();
}

void JoystickSelector::add_joystick(GamepadController::Joystick new_joystick, QIcon *icon) {
  this->m_joysticks.push_back({new_joystick, icon});
  this->refresh();
}

void JoystickSelector::set_current_joystick(GamepadController::Joystick joystick) {
  auto it = std::find_if(this->m_joysticks.begin(), this->m_joysticks.end(),
                         [&](const JoystickSelector::Item &i) { return i.joystick == joystick; });
  if (it != this->m_joysticks.end()) {
    m_selected_joystick_idx = distance(this->m_joysticks.begin(), it);
    m_selected_joystick = *it;
    updateIcon();
    emit joystickChanged(m_selected_joystick.joystick);
  }
}

void JoystickSelector::updateIcon() {
  if (m_icon_label) {
    m_icon_label->setPixmap(m_selected_joystick.icon->pixmap(QSize(40, 40)));
  }
}

void JoystickSelector::refresh() {

  if (m_joysticks.empty()) {
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

  if (m_selected_joystick.icon == nullptr && !m_joysticks.empty()) {
    m_selected_joystick_idx = 0;
    m_selected_joystick = m_joysticks[0];
  }

  QHBoxLayout *layout = new QHBoxLayout(this);

  m_icon_label = new QLabel(this);
  m_icon_label->setPixmap(m_selected_joystick.icon->pixmap(QSize(40, 40)));

  QPushButton *back_btn = new QPushButton(this);
  back_btn->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaSeekBackward));
  back_btn->setIconSize(QSize(30, 30));

  QPushButton *next_btn = new QPushButton(this);
  next_btn->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaSeekForward));
  next_btn->setIconSize(QSize(30, 30));

  QObject::connect(back_btn, &QPushButton::pressed, [this]() {
    if (m_joysticks.empty())
      return;
    m_selected_joystick_idx = (m_selected_joystick_idx + m_joysticks.size() - 1) % m_joysticks.size();
    m_selected_joystick = m_joysticks[m_selected_joystick_idx];
    updateIcon();
    emit joystickChanged(m_selected_joystick.joystick);
  });

  QObject::connect(next_btn, &QPushButton::pressed, [this]() {
    if (m_joysticks.empty())
      return;
    m_selected_joystick_idx = (m_selected_joystick_idx + 1) % m_joysticks.size();
    m_selected_joystick = m_joysticks[m_selected_joystick_idx];
    updateIcon();
    emit joystickChanged(m_selected_joystick.joystick);
  });

  layout->addWidget(back_btn);
  layout->addWidget(m_icon_label);
  layout->addWidget(next_btn);
  layout->addStretch(1);
}