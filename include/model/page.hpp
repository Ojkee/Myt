#ifndef STATE_HPP
#define STATE_HPP

#include <optional>
#include <string>
#include <unordered_map>

#include "data_cell.hpp"

using CellMap = std::unordered_map<CellPos, DataCell>;

class Page {
 public:
  Page();

  [[nodiscard]] bool is_running() noexcept { return m_is_running; }
  void stop_running() noexcept { m_is_running = false; }
  [[nodiscard]] bool cell_exists(const CellPos& pos) const noexcept;
  [[nodiscard]] std::optional<std::string> get_cell_raw_content(
      const CellPos& pos) const noexcept;

  void eval_cell(const CellPos& pos) noexcept;
  void erase_cell(const CellPos& pos) noexcept;

 private:
  void save_eval_cell();

  bool m_is_running{true};
  CellMap m_cells;
};

#endif  // !STATE_HPP
