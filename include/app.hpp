#ifndef APP_HPP
#define APP_HPP

#include "controller/input_handler.hpp"
#include "view/window.hpp"

class MytApp {
 public:
  MytApp();
  ~MytApp();
  void run() noexcept;

 private:
  InputHandler m_input_handler;
  Window m_window;
};

#endif  // !APP_HPP
