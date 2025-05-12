#ifndef STATE_HPP
#define STATE_HPP

#include <optional>
#include <string>
#include <unordered_map>

#include "data_cell.hpp"
#include "myt_lang/cell_pos.hpp"

using CellMap = std::unordered_map<CellPos, DataCell>;

class Page {
 public:
  Page() = default;

  [[nodiscard]] auto is_running() noexcept -> bool { return m_is_running; }
  auto stop_running() noexcept -> void { m_is_running = false; }
  [[nodiscard]] auto cell_exists(const CellPos& pos) const noexcept -> bool;
  [[nodiscard]] auto get_cell_raw_content(const CellPos& pos) const noexcept
      -> std::optional<std::string>;

  auto eval_cell(const CellPos& pos) noexcept -> void;
  auto erase_cell(const CellPos& pos) noexcept -> void;

 private:
  void save_eval_cell();

  bool m_is_running{true};
  CellMap m_cells{};
};

#endif  // !STATE_HPP
