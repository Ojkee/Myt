#ifndef APP_HPP
#define APP_HPP

#include "controller/input_handler.hpp"
#include "view/window.hpp"

class MytApp {
 public:
  MytApp() = default;

  auto run() noexcept -> void;

 private:
  Window m_window{};
  InputHandler m_input_handler{};
};

#endif  // !APP_HPP
