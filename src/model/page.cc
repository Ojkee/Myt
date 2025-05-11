#include "../../include/model/page.hpp"

#include <variant>

#include "../../include/model/myt_lang/lexer.hpp"
#include "../../include/model/myt_lang/parser.hpp"

std::optional<std::string> Page::get_cell_raw_content(
    const CellPos& pos) const noexcept {
  if (cell_exists(pos)) {
    return m_cells.at(pos).get_raw_content();
  }
  return std::nullopt;
}

bool Page::cell_exists(const CellPos& pos) const noexcept {
  return m_cells.find(pos) != m_cells.end();
}

void Page::eval_cell(const CellPos& pos) noexcept {
  if (!cell_exists(pos)) {
    return;
  }

  const auto raw_cell_data = m_cells.at(pos).get_raw_content();
  auto tokens = Lexer::tokenize(raw_cell_data);
  auto parse_result = Parser::parse(tokens);
  if (std::holds_alternative<ParsingError>(parse_result)) {
    // TODO:
    // auto err = std::get<ParsingError>(parse_result);
    // m_cells.at(pos).set_eval_content(MytObject());
    return;
  }

  // TODO: parse -> eval -> save to cell
}

void Page::erase_cell(const CellPos& pos) noexcept {
  if (!cell_exists(pos)) return;
  m_cells.erase(pos);
}
