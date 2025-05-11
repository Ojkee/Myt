#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <raylib.h>

class Window {
 public:
  explicit Window();
  explicit Window(int height, int width);

  auto should_close() const noexcept -> bool {
    return WindowShouldClose() && m_run;
  }
  auto close() noexcept -> void { m_run = false; }
  auto render() const noexcept -> void;

 private:
  auto init_window() noexcept -> void;

  bool m_run{true};
  const int m_height{800};
  const int m_width{800};

  // CONSTS
  const Color MYT_DARK_GRAY = Color{51, 51, 51, 255};
  const Font MYT_FONT =
      LoadFontEx("../../assets/font/Raleway-Light.ttf", 32, 0, 255);
};

#endif  // !WINDOW_HPP
