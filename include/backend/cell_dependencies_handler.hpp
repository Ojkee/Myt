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

  auto update_dependencies(const CellPos& affected_pos,
                           const ParsingResult& parsing_result) noexcept
      -> void;
  auto flush_dependencies() noexcept -> void {
    m_dependencies.clear();
    m_dependencies_uses.clear();
  }
  [[nodiscard]] auto get_dependencies() const noexcept -> const Dependencies {
    return m_dependencies;
  };
  [[nodiscard]] auto get_dependencies_uses() const noexcept
      -> const Dependencies {
    return m_dependencies_uses;
  };

 private:
  auto traverse_expression(const CellPos& affected_pos,
                           const Expression& expr) noexcept -> void;
  auto clear_dependencies_row(const CellPos& pos) noexcept -> void;
  auto append_dependency(const CellPos& value_pos, const CellPos& key_pos,
                         Dependencies& deps) noexcept -> void;
  static auto is_in_dependencies(const CellPos& key_pos,
                                 const Dependencies& deps) noexcept -> bool;
  [[nodiscard]] auto col_idx_to_letter_str(
      const CellLimitType& n) const noexcept -> std::string;

  Dependencies m_dependencies;       // {1, 1}: '=B3*3' => B3: {A1} AFFECTS
  Dependencies m_dependencies_uses;  // {1, 1}: '=B3*3' => A1: {B3} USES
};

#endif  // !CELL_DEPENDENCIES_HANDLER_HPP
