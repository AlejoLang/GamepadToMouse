#include "AppWindow.hpp"
#include "ControllersHandler.hpp"
#include "SDL3/SDL.h"
#include <QtCore/QTimer>
#include <filesystem>

#define SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS "SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"

void process_gamepad_events(AppWindow *app, ControllersHandler *handler);

extern const std::filesystem::path config_path =
    std::filesystem::path(std::getenv("HOME")) / ".config" / "ControllerMouse";

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK);

  if (!std::filesystem::exists(config_path)) {
    std::filesystem::create_directories(config_path);
  }

  ControllersHandler *gamepads_controller_handler = new ControllersHandler();
  AppWindow *app = new AppWindow(argc, argv, gamepads_controller_handler);

  QTimer *timer = new QTimer();
  QObject::connect(timer, &QTimer::timeout, [&]() {
    process_gamepad_events(app, gamepads_controller_handler);
    if (gamepads_controller_handler->get_current_controller() != nullptr) {
      gamepads_controller_handler->get_current_controller()->process_joysticks();
    }
  });

  timer->start(4);
  app->run();

  delete gamepads_controller_handler;
  delete timer;
  delete app;
  return 0;
}

void process_gamepad_events(AppWindow *app, ControllersHandler *handler) {
  SDL_Event new_event;
  bool has_event = SDL_PollEvent(&new_event);
  while (has_event) {
    switch (new_event.type) {
    case SDL_EVENT_GAMEPAD_ADDED: {
      handler->add_controller(new_event.gdevice.which);
      app->update_menu();
      break;
    }
    case SDL_EVENT_GAMEPAD_REMOVED: {
      handler->remove_controller(new_event.gdevice.which);
      app->update_menu();
      break;
    }
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP: {
      if (app->wants_gamepad_input()) {
        app->set_gamepad_input(new_event.gdevice.which, static_cast<SDL_GamepadButton>(new_event.gbutton.button));
      } else {
        handler->get_current_controller()->process_key_event(&new_event);
      }
      break;
    }
    default:
      break;
    }
    has_event = SDL_PollEvent(&new_event);
  }
}