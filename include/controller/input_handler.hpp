#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "../model/page.hpp"

class InputHandler {
 public:
  explicit InputHandler();

  void check_event();

 private:
  Page m_page = Page();
};

#endif  // !INPUT_HANDLER_HPP
