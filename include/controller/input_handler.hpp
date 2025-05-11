#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "../model/page.hpp"

class InputHandler {
 public:
  explicit InputHandler();

  auto check_event() -> void;

 private:
  Page m_page{};
};

#endif  // !INPUT_HANDLER_HPP
