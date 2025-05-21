#include "../../../include/backend/myt_lang/ast.hpp"

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
