#include "../../include/view/window.hpp"

#include <raylib.h>

Window::Window() { init_window(); }
Window::Window(int height, int width) : m_height(height), m_width(width) {
  init_window();
}

void Window::render() noexcept {
  BeginDrawing();
  ClearBackground(MYT_DARK_GRAY);
  DrawText("Myt placeholder", 300, 300, 20, LIGHTGRAY);
  EndDrawing();
}

void Window::init_window() noexcept {
  InitWindow(m_width, m_height, "My calc sheet");
}
