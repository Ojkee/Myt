#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "../model/state.hpp"

class InputHandler {
 public:
  InputHandler();
  void check_event();

 private:
  State m_state;
};

#endif  // !INPUT_HANDLER_HPP
