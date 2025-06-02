#include "../../../include/backend/myt_lang/ast.hpp"

#include "backend/myt_lang/evaluator.hpp"

auto AstUtils::token_to_precendece(const TokenType& type) noexcept
    -> Precendence {
  switch (type) {
    case TokenType::Eq:
    case TokenType::NotEq:
      return Precendence::Equals;
    case TokenType::Gt:
    case TokenType::Ge:
    case TokenType::Lt:
    case TokenType::Le:
      return Precendence::LessGreater;
    case TokenType::Plus:
    case TokenType::Minus:
      return Precendence::Sum;
    case TokenType::Slash:
    case TokenType::Asterisk:
      return Precendence::Product;
    case TokenType::Colon:
      return Precendence::CellRange;
    case TokenType::LParen:
      return Precendence::Call;
    default:
      return Precendence::Lowest;
  }
}

auto AstUtils::same_hash_code(const Expression& lhs,
                              const Expression& rhs) noexcept -> bool {
  return typeid(lhs).hash_code() == typeid(rhs).hash_code();
}

auto ExpressionCellRange::generate_range() const noexcept
    -> const std::vector<CellPos> {
  auto lhs_cell = dynamic_cast<const ExpressionCell*>(&get_lhs_expression());
  auto rhs_cell = dynamic_cast<const ExpressionCell*>(&get_rhs_expression());
  if (lhs_cell == nullptr || rhs_cell == nullptr) {
    return {};
  }

  const auto lhs_pos_str = lhs_cell->get_cell_token().literal;
  const auto rhs_pos_str = rhs_cell->get_cell_token().literal;

  const auto pos_a = CellPos{lhs_pos_str};
  const auto pos_b = CellPos{rhs_pos_str};

  const auto [begin, end] = (pos_a < pos_b) ? std::make_tuple(pos_a, pos_b)
                                            : std::make_tuple(pos_b, pos_a);

  std::vector<CellPos> ret_val{};
  for (auto i{begin.col}; i <= end.col; ++i) {
    for (auto j{begin.row}; j <= end.row; ++j) {
      ret_val.emplace_back(CellPos{i, j});
    }
  }
  return ret_val;
}

auto ExpressionCellRange::get_cells_strs() const noexcept
    -> const std::tuple<std::string, std::string> {
  auto lhs_cell = dynamic_cast<const ExpressionCell*>(&get_lhs_expression());
  auto rhs_cell = dynamic_cast<const ExpressionCell*>(&get_rhs_expression());
  if (lhs_cell == nullptr || rhs_cell == nullptr) {
    return {};
  }

  const auto lhs_pos_str = lhs_cell->get_cell_token().literal;
  const auto rhs_pos_str = rhs_cell->get_cell_token().literal;

  const auto pos_a = CellPos{lhs_pos_str};
  const auto pos_b = CellPos{rhs_pos_str};

  const auto [lhs_str, rhs_str] =
      (pos_a < pos_b) ? std::make_tuple(lhs_pos_str, rhs_pos_str)
                      : std::make_tuple(rhs_pos_str, lhs_pos_str);
  return {lhs_str, rhs_str};
}
