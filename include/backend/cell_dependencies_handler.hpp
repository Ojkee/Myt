#ifndef CELL_DEPENDENCIES_HANDLER_HPP
#define CELL_DEPENDENCIES_HANDLER_HPP

#include <unordered_map>
#include <unordered_set>

#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/parser.hpp"

using AffectedCells = std::unordered_set<CellPos>;
using Dependencies = std::unordered_map<CellPos, AffectedCells>;

class DependenciesHandler {
 public:
  explicit DependenciesHandler() : m_dependencies() {}

  auto catch_dependencies(const CellPos& affected_pos,
                          const ParsingResult& parsing_result) noexcept -> void;
  auto flush_dependencies() noexcept -> void { m_dependencies.clear(); }

  [[nodiscard]] auto get_dependencies() const noexcept -> const Dependencies {
    return m_dependencies;
  };

 private:
  auto traverse_expression(const CellPos& affected_pos,
                           const Expression& expr) noexcept -> void;
  auto append_dependency(const CellPos& affected_pos,
                         const CellPos& used_pos) noexcept -> void;
  auto is_in_dependencies(const CellPos& used_pos) const noexcept -> bool;
  [[nodiscard]] auto col_idx_to_letter_str(
      const CellLimitType& n) const noexcept -> std::string;

  [[nodiscard]] static auto generate_cell_range(const CellPos& begin,
                                                const CellPos& end) noexcept
      -> std::vector<CellPos> {
    std::vector<CellPos> ret_val{};
    for (auto i{begin.col}; i <= end.col; ++i) {
      for (auto j{begin.row}; j <= end.row; ++j) {
        const auto pos = CellPos{i, j};
        ret_val.emplace_back(pos);
      }
    }
    return ret_val;
  }

  Dependencies m_dependencies;
};

#endif  // !CELL_DEPENDENCIES_HANDLER_HPP
