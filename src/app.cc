#include "../include/app.hpp"

auto MytApp::run() noexcept -> void {
  while (!m_window.should_close()) {
    m_input_handler.check_event();
    m_window.render();
  }
}
