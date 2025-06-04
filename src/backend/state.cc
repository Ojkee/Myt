#include "../../include/backend/state.hpp"

#include <qobject.h>
#include <qtmetamacros.h>

#include <array>
#include <cstddef>
#include <iostream>
#include <string>

#include "backend/data_cell.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/evaluator.hpp"
#include "backend/myt_lang/lexer.hpp"
#include "backend/myt_lang/myt_object.hpp"
#include "backend/myt_lang/parser.hpp"
#include "global_utils/global_utils.hpp"

State::State(QObject* parent)
    : QObject(parent), m_pages({Page{}}), m_dependencies_handler() {}

auto State::get_content_by_pos(const CellLimitType& col,
                               const CellLimitType& row) noexcept -> QString {
  const CellPos pos{col, row};
  const auto current_page = m_pages.at(m_current_page_idx);
  const auto cell = current_page.get_cell_eval_content(pos);
  return QString::fromStdString(cell.value_or(""));
}

auto State::get_raw_content_by_pos(const CellLimitType& col,
                                   const CellLimitType& row) noexcept
    -> QString {
  const CellPos pos{col, row};
  const auto& current_page = m_pages.at(m_current_page_idx);
  const auto raw_content_str =
      current_page.get_cell_raw_content(pos).value_or("");
  return QString::fromStdString(raw_content_str);
}

auto State::eval_save(const QString& raw_content,
                      const CellLimitType& col,
                      const CellLimitType& row) noexcept -> void {
  const auto raw_content_str = raw_content.toStdString();
  evaluate(raw_content_str, CellPos{col, row});
}

auto State::log_cells() const noexcept -> void {
  const auto& current_page = m_pages.at(m_current_page_idx);
  for (const auto& [pos, dc] : current_page.get_cells()) {
    std::cout << pos.col << ":" << pos.row << ": `" << dc.to_string() << "`\n";
  }
}

auto State::flush_dependencies() noexcept -> void {
  m_dependencies_handler.flush_dependencies();
}

auto State::get_dependencies() const noexcept
    -> const DependenciesHandler::Dependencies {
  return m_dependencies_handler.get_dependencies();
}

auto State::get_dependencies_uses() const noexcept
    -> const DependenciesHandler::Dependencies {
  return m_dependencies_handler.get_dependencies_uses();
}

void State::setEditingCol(int col) {
  if (col != m_editingCol && GlobalUtils::is_in_cell_range(col)) {
    m_editingCol = col;
    emit editingColChanged();
  }
}

void State::setEditingRow(int row) {
  if (row != m_editingRow && GlobalUtils::is_in_cell_range(row)) {
    m_editingRow = row;
    emit editingRowChanged();
  }
}

auto State::evaluate(const std::string& content, const CellPos& pos) noexcept
    -> void {
  const auto cells = m_pages.at(m_current_page_idx).get_cells();
  const auto tokens = Lexer::tokenize(content);
  const auto parsed = Parser::parse(tokens);

  m_dependencies_handler.update_dependencies(pos, parsed);

  const auto cyclic_pos = m_dependencies_handler.catch_circling_cells_DFS();
  if (!cyclic_pos.empty()) {
    m_dependencies_handler.filter_cyclic_dependencies(cyclic_pos);
    set_cyclic_dependencies_errors(cyclic_pos);
    for (const auto& c_pos : cyclic_pos) {
      reeval_affected(c_pos);
    }
    return;
  }

  const auto obj = Evaluator::evaluate(parsed, cells);
  const auto data_cell = DataCell{content, obj};
  save_data_cell(pos, data_cell);
  reeval_affected(pos);
}

auto State::save_data_cell(const CellPos& pos,
                           const DataCell& data_cell) noexcept -> void {
  auto& current_page = m_pages.at(m_current_page_idx);
  current_page.save_cell(data_cell, pos);
}

template <class Container>
auto State::set_cyclic_dependencies_errors(const Container& positions) noexcept
    -> void {
  const auto positions_str = build_cell_pos_str(positions);
  const auto err_msg = "CYCLE: " + positions_str;
  const auto err_obj = Evaluator::get_error_obj(err_msg);
  const auto cells = m_pages.at(m_current_page_idx).get_cells();

  for (const auto& pos : positions) {
    if (cells.find(pos) == cells.cend()) {
      continue;
    }
    const auto data_cell = cells.at(pos);
    const auto content = data_cell.get_raw_content();
    const auto data_cell_err = DataCell{content, err_obj};
    save_data_cell(pos, data_cell_err);
  }
}

template <class Container>
auto State::build_cell_pos_str(const Container& c) -> std::string {
  std::string res{""};
  std::size_t i{0};
  for (const auto& pos : c) {
    res += pos.to_string();
    if (i < c.size() - 1) {
      res += ", ";
    }
    ++i;
  }
  return "(" + res + ")";
}

auto State::reeval_affected(const CellPos& pos) noexcept -> void {
  using PosSet = std::unordered_set<CellPos>;
  const auto affected_positions =
      m_dependencies_handler.get_affected_positions(pos).value_or(PosSet{});

  const auto cells = m_pages.at(m_current_page_idx).get_cells();
  for (const auto& affected_pos : affected_positions) {
    const auto data_cell = cells.at(affected_pos);
    const auto content = data_cell.get_raw_content();
    evaluate(content, affected_pos);
    emit requestCellUpdate(affected_pos.col, affected_pos.row);
  }
}
