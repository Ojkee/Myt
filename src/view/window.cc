#include "../../include/view/window.hpp"

#include <raylib.h>

Window::Window() { init_window(); }
Window::Window(int height, int width) : m_height(height), m_width(width) {
  init_window();
}

auto Window::render() const noexcept -> void {
  BeginDrawing();
  ClearBackground(MYT_DARK_GRAY);
  DrawText("Myt placeholder", 300, 300, 20, LIGHTGRAY);
  EndDrawing();
}

auto Window::init_window() noexcept -> void {
  InitWindow(m_width, m_height, "My calc sheet");
}
