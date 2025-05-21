#ifndef APP_HPP
#define APP_HPP

#include "frontend/window.hpp"

class MytApp {
 public:
  MytApp() = delete;
  MytApp(int argc, char *argv[]) : m_window(argc, argv) {};

  auto exec() noexcept -> int;

 private:
  Window m_window;
};

#endif  // !APP_HPP
