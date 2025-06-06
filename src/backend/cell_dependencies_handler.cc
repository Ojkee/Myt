#include "../../include/backend/cell_dependencies_handler.hpp"

#include <cassert>

#include "backend/myt_lang/ast.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/parser.hpp"
#include "backend/myt_lang/token.hpp"

auto DependenciesHandler::update_dependencies(
    const CellPos& affected_pos,
    const ParsingResult& parsing_result) noexcept -> void {
  clear_dependencies_pos(affected_pos);
  if (std::holds_alternative<ParsingError>(parsing_result)) {
    return;
  }
  const auto& expr = std::get<ExpressionSharedPtr>(parsing_result);
  traverse_expression(affected_pos, *expr);
}

auto DependenciesHandler::flush_dependencies() noexcept -> void {
  m_dependencies.clear();
  m_dependencies_uses.clear();
}

auto DependenciesHandler::get_dependencies() const noexcept
    -> const Dependencies {
  return m_dependencies;
};

auto DependenciesHandler::get_dependencies_uses() const noexcept
    -> const Dependencies {
  return m_dependencies_uses;
};

auto DependenciesHandler::get_affected_positions(const CellPos& pos)
    const noexcept -> const std::optional<std::unordered_set<CellPos>> {
  if (!is_in_dependencies(pos, m_dependencies))
    return std::nullopt;
  return m_dependencies.at(pos);
}

auto DependenciesHandler::clear_dependencies_pos(const CellPos& pos) noexcept
    -> void {
  if (!is_in_dependencies(pos, m_dependencies_uses)) {
    return;
  }
  {
    auto& used = m_dependencies_uses.at(pos);
    for (const auto& used_pos : used) {
      m_dependencies.at(used_pos).erase(pos);
      if (m_dependencies.at(used_pos).empty()) {
        m_dependencies.erase(used_pos);
      }
    }
  }
  m_dependencies_uses.erase(pos);
}

auto DependenciesHandler::traverse_expression(const CellPos& affected_pos,
                                              const Expression& expr) noexcept
    -> void {
  if (auto expr_cell_range = dynamic_cast<const ExpressionCellRange*>(&expr)) {
    const auto cell_range = expr_cell_range->generate_range();
    for (const auto& pos : cell_range) {
      append_dependency(affected_pos, pos, m_dependencies);
      append_dependency(pos, affected_pos, m_dependencies_uses);
    }
  } else if (auto infix = dynamic_cast<const ExpressionInfix*>(&expr)) {
    const auto& lhs = infix->get_lhs_expression();
    const auto& rhs = infix->get_rhs_expression();
    traverse_expression(affected_pos, lhs);
    traverse_expression(affected_pos, rhs);
  } else if (auto prefix = dynamic_cast<const ExpressionPrefix*>(&expr)) {
    const auto& rhs = prefix->get_expression();
    traverse_expression(affected_pos, rhs);
  } else if (auto cell = dynamic_cast<const ExpressionCell*>(&expr)) {
    const auto cell_token = cell->get_cell_token();
    const auto used_pos = CellPos{cell_token.literal};
    if (used_pos == affected_pos) {
      return;
    }
    append_dependency(affected_pos, used_pos, m_dependencies);
    append_dependency(used_pos, affected_pos, m_dependencies_uses);
  } else if (auto call = dynamic_cast<const ExpressionFnCall*>(&expr)) {
    for (const auto& arg : call->get_arguments()) {
      traverse_expression(affected_pos, *arg);
    }
  }
}

auto DependenciesHandler::append_dependency(const CellPos& value_pos,
                                            const CellPos& key_pos,
                                            Dependencies& deps) noexcept
    -> void {
  if (is_in_dependencies(key_pos, deps)) {
    auto& value_positions = deps.at(key_pos);
    value_positions.insert(value_pos);
    return;
  }
  deps.insert({key_pos, {value_pos}});
}

auto DependenciesHandler::is_in_dependencies(const CellPos& key_pos,
                                             const Dependencies& deps) noexcept
    -> bool {
  return deps.find(key_pos) != deps.cend();
}

auto DependenciesHandler::catch_circling_cells_DFS() const noexcept
    -> std::unordered_set<CellPos> {
  std::unordered_map<CellPos, VisitState> visit_states;
  std::unordered_set<CellPos> cycled;

  for (const auto& [node, _] : m_dependencies) {
    if (visit_states[node] == VisitState::Unvisited) {
      std::vector<CellPos> path_stack;
      dfs_visit(node, visit_states, cycled, path_stack);
    }
  }

  return cycled;
}

auto DependenciesHandler::filter_cyclic_dependencies(
    const CellPosSet& cycled) noexcept -> void {
  for (const auto& pos : cycled) {
    clear_dependencies_pos(pos);
  }
}

auto DependenciesHandler::dfs_visit(
    const CellPos& node,
    std::unordered_map<CellPos, VisitState>& visit_states,
    std::unordered_set<CellPos>& cycled,
    std::vector<CellPos>& path_stack) const noexcept -> void {
  visit_states[node] = VisitState::Visiting;
  path_stack.push_back(node);

  if (auto it = m_dependencies.find(node); it != m_dependencies.end()) {
    for (const CellPos& neighbor : it->second) {
      if (visit_states[neighbor] == VisitState::Visiting) {
        auto it = std::find(path_stack.begin(), path_stack.end(), neighbor);
        if (it != path_stack.end()) {
          for (; it != path_stack.end(); ++it) {
            cycled.insert(*it);
          }
        }
      } else if (visit_states[neighbor] == VisitState::Unvisited) {
        dfs_visit(neighbor, visit_states, cycled, path_stack);
      }
    }
  }

  path_stack.pop_back();
  visit_states[node] = VisitState::Visited;
}
