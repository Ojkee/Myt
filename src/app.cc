#include "../include/app.hpp"

MytApp::MytApp() : m_input_handler(), m_window() {}

MytApp::~MytApp() {}

void MytApp::run() noexcept {
  while (!m_window.should_close()) {
    m_input_handler.check_event();
    m_window.render();
  }
}
