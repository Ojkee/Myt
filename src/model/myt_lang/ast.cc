#include "../../../include/model/myt_lang/ast.hpp"

Precendence AstUtils::token_to_precendece(const TokenType& type) noexcept {
  switch (type) {
    case TokenType::Eq:
    case TokenType::NotEq:
      return Precendence::Equals;
    case TokenType::Lt:
    case TokenType::Gt:
      return Precendence::LessGreater;
    case TokenType::Plus:
    case TokenType::Minus:
      return Precendence::Sum;
    case TokenType::Slash:
    case TokenType::Asterisk:
      return Precendence::Product;
    default:
      return Precendence::Lowest;
  }
}
