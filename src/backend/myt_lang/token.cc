#include "../../../include/backend/myt_lang/token.hpp"

#include <cassert>
#include <cstdio>

const std::string token_type_to_string(const TokenType& token_type) noexcept {
  switch (token_type) {
    case TokenType::Illegal:
      return "Illegal";
    case TokenType::EndOfCell:
      return "EndOfCell";
    case TokenType::Identifier:
      return "Identifier";
    case TokenType::CellIdentifier:
      return "CellIdentifier";
    case TokenType::Bool:
      return "Bool";
    case TokenType::Int:
      return "Int";
    case TokenType::Float:
      return "Float";
    case TokenType::String:
      return "String";
    case TokenType::Nil:
      return "Nil";
    case TokenType::Assign:
      return "Assign";
    case TokenType::Bang:
      return "Bang";
    case TokenType::Plus:
      return "Plus";
    case TokenType::Minus:
      return "Minus";
    case TokenType::Asterisk:
      return "Asterisk";
    case TokenType::Slash:
      return "Slash";
    case TokenType::Comma:
      return "Comma";
    case TokenType::Colon:
      return "Colon";
    case TokenType::LParen:
      return "LParen";
    case TokenType::RParen:
      return "RParen";
    case TokenType::Eq:
      return "==";
    case TokenType::NotEq:
      return "!=";
    case TokenType::Gt:
      return ">";
    case TokenType::Lt:
      return "<";
    case TokenType::Ge:
      return ">=";
    case TokenType::Le:
      return "<=";
    default:
      assert(false && "Unimplemented token_type to string");
  }
  return "Unimplemented Token Type";
}

Token::operator std::string() const {
  return token_type_to_string(type) + "(" + literal + ")";
}

bool Token::operator==(const Token& rhs) const {
  return type == rhs.type && literal == rhs.literal;
}
