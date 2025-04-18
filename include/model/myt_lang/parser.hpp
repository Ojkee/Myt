#ifndef PARSER_HPP
#define PARSER_HPP

#include <functional>
#include <memory>
#include <variant>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

using ExpressionPtr = std::unique_ptr<Expression>;
using ParsingResult = std::variant<ExpressionPtr, ParsingError>;
using Tokens = std::vector<Token>;
using PrefixFn = std::function<ExpressionPtr(Tokens::iterator&)>;
using InfixFn = std::function<ExpressionPtr(ExpressionPtr, Tokens::iterator&)>;

class Parser {
 public:
  [[nodiscard]] static ParsingResult parse(const Tokens& tokens) noexcept;

 private:
  template <class FnType>
  [[nodiscard]] static bool is_in_fns(
      const std::unordered_map<TokenType, FnType>& fns,
      const TokenType& type) noexcept {
    return fns.find(type) != fns.end();
  }
  [[nodiscard]] const static std::string concat_token_literals(
      const Tokens& tokens) noexcept;

  // PREFIX
  [[nodiscard]] static ExpressionPtr parse_identifier(Tokens::iterator& it);
  inline static const std::unordered_map<TokenType, PrefixFn> prefixFns = {
      {TokenType::Identifier, parse_identifier},
  };

  // INFIX
  inline static const std::unordered_map<TokenType, InfixFn> infixFns = {};
};

#endif  // !PARSER_HPP
