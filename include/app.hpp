#ifndef APP_HPP
#define APP_HPP

#include "controller/input_handler.hpp"
#include "view/window.hpp"

class MytApp {
 public:
  MytApp();

  void run() noexcept;

 private:
  Window m_window;
  InputHandler m_input_handler;
};

#endif  // !APP_HPP
