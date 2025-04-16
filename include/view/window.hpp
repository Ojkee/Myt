#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <raylib.h>

class Window {
 public:
  explicit Window();
  explicit Window(int height, int width);

  bool should_close() { return WindowShouldClose() && m_run; }
  void close() { m_run = false; }
  void render() noexcept;

 private:
  void init_window() noexcept;

  bool m_run{true};
  int m_height{800};
  int m_width{800};

  // CONSTS
  const Color MYT_DARK_GRAY = Color{51, 51, 51, 255};
  const Font MYT_FONT =
      LoadFontEx("../../assets/font/Raleway-Light.ttf", 32, 0, 255);
};

#endif  // !WINDOW_HPP
