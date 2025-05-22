#ifndef PAGE_HPP
#define PAGE_HPP

#include <qobject.h>
#include <qtmetamacros.h>
#include <qurl.h>

#include <optional>
#include <string>
#include <unordered_map>

#include "data_cell.hpp"
#include "myt_lang/cell_pos.hpp"

using CellMap = std::unordered_map<CellPos, DataCell>;

class Page {
 public:
  explicit Page() : m_cells() {}

  [[nodiscard]] auto cell_exists(const CellPos& pos) const noexcept -> bool;
  [[nodiscard]] auto get_cell_raw_content(const CellPos& pos) const noexcept
      -> std::optional<std::string>;
  [[nodiscard]] auto get_cell_eval_content(const CellPos& pos) const noexcept
      -> std::optional<std::string>;

  [[nodiscard]] auto get_cells() const noexcept -> CellMap { return m_cells; }
  auto save_cell(const DataCell& data_cell, const CellPos& pos) noexcept
      -> void {
    m_cells.insert_or_assign(pos, data_cell);
  };
  auto erase_cell(const CellPos& pos) noexcept -> void;

 private:
  CellMap m_cells;
};

#endif  // !PAGE_HPP
