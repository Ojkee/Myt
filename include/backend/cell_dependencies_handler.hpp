#ifndef CELL_DEPENDENCIES_HANDLER_HPP
#define CELL_DEPENDENCIES_HANDLER_HPP

#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/parser.hpp"

class DependenciesHandler {
 public:
  explicit DependenciesHandler() : m_dependencies() {}

  using CellPosSet = std::unordered_set<CellPos>;
  using Dependencies = std::unordered_map<CellPos, CellPosSet>;
  enum class VisitState { Unvisited, Visiting, Visited };

  auto update_dependencies(const CellPos& affected_pos,
                           const ParsingResult& parsing_result) noexcept
      -> void;
  auto flush_dependencies() noexcept -> void;
  [[nodiscard]] auto get_dependencies() const noexcept -> const Dependencies;
  [[nodiscard]] auto get_dependencies_uses() const noexcept
      -> const Dependencies;
  [[nodiscard]] auto get_affected_positions(const CellPos& pos) const noexcept
      -> const std::optional<CellPosSet>;
  [[nodiscard]] auto catch_circling_cells_DFS() const noexcept
      -> std::unordered_set<CellPos>;
  auto filter_cyclic_dependencies(const CellPosSet& cycled) noexcept -> void;

 private:
  auto traverse_expression(const CellPos& affected_pos,
                           const Expression& expr) noexcept -> void;
  auto clear_dependencies_pos(const CellPos& pos) noexcept -> void;
  auto append_dependency(const CellPos& value_pos,
                         const CellPos& key_pos,
                         Dependencies& deps) noexcept -> void;
  [[nodiscard]] static auto is_in_dependencies(
      const CellPos& key_pos,
      const Dependencies& deps) noexcept -> bool;
  auto dfs_visit(const CellPos& node,
                 std::unordered_map<CellPos, VisitState>& visit_states,
                 std::unordered_set<CellPos>& cycled,
                 std::vector<CellPos>& path_stack) const noexcept -> void;

  Dependencies m_dependencies;       // {1, 1}: '=B3*3' => B3: {A1} AFFECTS
  Dependencies m_dependencies_uses;  // {1, 1}: '=B3*3' => A1: {B3} USES
};

#endif  // !CELL_DEPENDENCIES_HANDLER_HPP
