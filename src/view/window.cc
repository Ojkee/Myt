#include "../../include/view/window.hpp"

#include <raylib.h>

Window::Window() : m_height(800), m_width(800) { init_window(); }
Window::Window(int height, int width) : m_height(height), m_width(width) {
  init_window();
}

void Window::render() noexcept {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawText("Hello, Raylib!", 190, 200, 20, LIGHTGRAY);
  EndDrawing();
}

void Window::init_window() noexcept {
  InitWindow(m_width, m_height, "My calc sheet");
}
