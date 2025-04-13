#include "../../include/controller/input_handler.hpp"

#include "raylib.h"

InputHandler::InputHandler() { m_state = State(); }

void InputHandler::check_event() {
  if (IsKeyPressed(KEY_Q)) {
    m_state.stop_running();
  }
}
