#include "../include/app.hpp"

MytApp::MytApp() : m_window(), m_input_handler() {}

void MytApp::run() noexcept {
  while (!m_window.should_close()) {
    m_input_handler.check_event();
    m_window.render();
  }
}
