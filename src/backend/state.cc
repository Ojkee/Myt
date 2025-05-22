#include "../../include/backend/state.hpp"

#include <qobject.h>

#include <iostream>

#include "backend/data_cell.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/evaluator.hpp"
#include "backend/myt_lang/lexer.hpp"
#include "backend/myt_lang/myt_object.hpp"
#include "backend/myt_lang/parser.hpp"

State::State(QObject* parent) : QObject(parent), m_pages({Page{}}) {}

auto State::get_content_by_pos(const CellLimitType& col,
                               const CellLimitType& row) noexcept -> QString {
  const CellPos pos{col, row};
  const auto current_page = m_pages.at(m_current_page_idx);
  const auto cell = current_page.get_cell_eval_content(pos);
  return QString::fromStdString(cell.value_or(""));
}

QString State::get_raw_content_by_pos(const CellLimitType& col,
                                      const CellLimitType& row) noexcept {
  const CellPos pos{col, row};
  const auto& current_page = m_pages.at(m_current_page_idx);
  const auto raw_content_str =
      current_page.get_cell_raw_content(pos).value_or("");
  return QString::fromStdString(raw_content_str);
}

auto State::eval_save(const QString& raw_content, const CellLimitType& col,
                      const CellLimitType& row) noexcept -> void {
  const auto raw_content_str = raw_content.toStdString();
  const auto obj = eval_content(raw_content_str);
  const auto data_cell = DataCell{raw_content_str, obj};
  const CellPos pos{col, row};
  auto& current_page = m_pages.at(m_current_page_idx);
  current_page.save_cell(data_cell, pos);
}

auto State::eval_content(const std::string& raw_content) const noexcept
    -> MytObjectPtr {
  const auto cells = m_pages.at(m_current_page_idx).get_cells();
  const auto tokens = Lexer::tokenize(raw_content);
  const auto parsed = Parser::parse(tokens);
  const auto obj = Evaluator::evaluate(parsed, cells);
  return obj;
}

void State::log_cells() const noexcept {
  const auto current_page = m_pages.at(m_current_page_idx);
  for (const auto& [pos, dc] : current_page.get_cells()) {
    std::cout << "{" << pos.col << ":" << pos.row << "} `" << dc.to_string()
              << "`\n";
  }
}
