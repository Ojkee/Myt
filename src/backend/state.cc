#include "../../include/backend/state.hpp"

#include <qobject.h>
#include <qtmetamacros.h>

#include <array>
#include <iostream>

#include "backend/data_cell.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/evaluator.hpp"
#include "backend/myt_lang/lexer.hpp"
#include "backend/myt_lang/myt_object.hpp"
#include "backend/myt_lang/parser.hpp"
#include "frontend/window_utils.hpp"

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

auto State::evaluate(const std::string& content, const CellPos& pos) noexcept
    -> void {
  const auto cells = m_pages.at(m_current_page_idx).get_cells();
  const auto tokens = Lexer::tokenize(content);
  const auto parsed = Parser::parse(tokens);
  const auto obj = Evaluator::evaluate(parsed, cells);
  const auto data_cell = DataCell{content, obj};
  save_data_cell(pos, data_cell);
  m_dependencies_handler.update_dependencies(pos, parsed);
  reeval_affected(pos);
}

auto State::save_data_cell(const CellPos& pos,
                           const DataCell& data_cell) noexcept -> void {
  auto& current_page = m_pages.at(m_current_page_idx);
  current_page.save_cell(data_cell, pos);
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
