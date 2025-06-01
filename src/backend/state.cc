#include "../../include/backend/state.hpp"

#include <qobject.h>

#include <array>
#include <iostream>

#include "backend/data_cell.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/evaluator.hpp"
#include "backend/myt_lang/lexer.hpp"
#include "backend/myt_lang/myt_object.hpp"
#include "backend/myt_lang/parser.hpp"

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

auto State::eval_save(const QString& raw_content, const CellLimitType& col,
                      const CellLimitType& row) noexcept -> void {
  const auto raw_content_str = raw_content.toStdString();
  const auto cells = m_pages.at(m_current_page_idx).get_cells();
  const auto tokens = Lexer::tokenize(raw_content_str);
  const auto parsed = Parser::parse(tokens);
  const auto obj = Evaluator::evaluate(parsed, cells);
  const auto data_cell = DataCell{raw_content_str, obj};
  save_data_cell({col, row}, data_cell);
  m_dependencies_handler.update_dependencies({col, row}, parsed);
}

auto State::log_cells() const noexcept -> void {
  const auto& current_page = m_pages.at(m_current_page_idx);
  for (const auto& [pos, dc] : current_page.get_cells()) {
    std::cout << "{" << pos.col << ":" << pos.row << "} `" << dc.to_string()
              << "`\n";
  }
}

auto State::save_data_cell(const CellPos& pos,
                           const DataCell& data_cell) noexcept -> void {
  auto& current_page = m_pages.at(m_current_page_idx);
  current_page.save_cell(data_cell, pos);
}
