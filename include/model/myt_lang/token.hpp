#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

enum class TokenType : uint8_t {
  Illegal,
  EndOfCell,

  Identifier,
  CellIdentifier,

  Bool,
  Int,
  Float,
  String,
  Nil,

  Assign,
  Bang,
  Plus,
  Minus,
  Asterisk,
  Slash,

  Comma,

  LParen,
  RParen,

  Eq,
  NotEq,
  Gt,
  Lt,
  Ge,
  Le,

  If,
};

namespace TokenUtils {
const std::string token_type_to_string(const TokenType& tokenType) noexcept;

static std::unordered_map<std::string, TokenType> keywordsLookup{
    {"true", TokenType::Bool},
    {"false", TokenType::Bool},
    {"if", TokenType::If},
};

}  // namespace TokenUtils

struct Token {
  const TokenType type;
  const std::string literal;

  operator std::string() const;
  bool operator==(const Token& rhs) const;
};

#endif  // !TOKEN_HPP
