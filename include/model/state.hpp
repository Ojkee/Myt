#ifndef STATE_HPP
#define STATE_HPP

class State {
 public:
  State();

  bool is_running() noexcept { return m_is_running; }
  void stop_running() noexcept { m_is_running = false; }

 private:
  bool m_is_running = true;
};

#endif  // !STATE_HPP
