#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <raylib.h>

class Window {
 public:
  Window();
  Window(int height, int width);

  bool should_close() { return WindowShouldClose(); }
  void render() noexcept;

 private:
  void init_window() noexcept;

  int m_height;
  int m_width;
};

#endif  // !WINDOW_HPP
