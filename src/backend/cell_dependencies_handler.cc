#include "../../include/backend/cell_dependencies_handler.hpp"

#include <cassert>
#include <string>

#include "backend/myt_lang/ast.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/parser.hpp"
#include "backend/myt_lang/token.hpp"

auto DependenciesHandler::update_dependencies(
    const CellPos& affected_pos, const ParsingResult& parsing_result) noexcept
    -> void {
  if (std::holds_alternative<ParsingError>(parsing_result)) {
    // TODO UPDATE DEPS FOR POS
    return;
  }
  const auto& expr = std::get<ExpressionSharedPtr>(parsing_result);
  clear_dependencies_row(affected_pos);
  traverse_expression(affected_pos, *expr);
}

auto DependenciesHandler::clear_dependencies_row(const CellPos& pos) noexcept
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

auto DependenciesHandler::col_idx_to_letter_str(
    const CellLimitType& n) const noexcept -> std::string {
  std::string acc = "";
  for (CellLimitType i = n; i > 0; i /= 26) {
    i -= 1;
    acc = (char)((i % 26) + 'A') + acc;
  }
  return acc;
}
