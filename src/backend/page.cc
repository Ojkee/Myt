#include "../../include/backend/page.hpp"

std::optional<std::string> Page::get_cell_raw_content(
    const CellPos& pos) const noexcept {
  if (!cell_exists(pos)) return std::nullopt;
  return m_cells.at(pos).get_raw_content();
}

auto Page::get_cell_eval_content(const CellPos& pos) const noexcept
    -> std::optional<std::string> {
  if (!cell_exists(pos)) return std::nullopt;
  return m_cells.at(pos).to_string();
}

bool Page::cell_exists(const CellPos& pos) const noexcept {
  return m_cells.find(pos) != m_cells.end();
}

void Page::erase_cell(const CellPos& pos) noexcept {
  if (!cell_exists(pos)) return;
  m_cells.erase(pos);
}
