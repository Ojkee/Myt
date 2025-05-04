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
using PrefixFn =
    std::function<ParsingResult(std::size_t& token_idx, const Tokens& tokens)>;
using InfixFn = std::function<ParsingResult(
    ExpressionPtr, std::size_t& token_idx, const Tokens& tokens)>;

class Parser {
 public:
  [[nodiscard]] static ParsingResult parse(const Tokens& tokens) noexcept;

 private:
  [[nodiscard]] static ParsingResult parse_expression(
      std::size_t& token_idx, const Tokens& tokens,
      const Precendence&& precendence) noexcept;
  [[nodiscard]] static ParsingResult parse_prefix_fn(
      std::size_t& token_idx, const Tokens& tokens) noexcept;

  [[nodiscard]] static ParsingResult parse_prefix_expression(
      std::size_t& token_idx, const Tokens& tokens) noexcept;
  [[nodiscard]] static ParsingResult parse_infix_expression(
      std::size_t& token_idx, const Tokens& tokens) noexcept;

  template <class FnType>
  [[nodiscard]] static bool is_in_fns(
      const std::unordered_map<TokenType, FnType>& fns,
      const TokenType& type) noexcept {
    return fns.find(type) != fns.end();
  }
  [[nodiscard]] const static std::string concat_token_literals(
      const std::size_t& start, const Tokens& tokens) noexcept;

  // PREFIX
  [[nodiscard]] static ExpressionPtr parse_identifier(std::size_t& token_idx,
                                                      const Tokens& tokens);
  [[nodiscard]] static ParsingResult parse_int_literal(std::size_t& token_idx,
                                                       const Tokens& tokens);
  [[nodiscard]] static ParsingResult parse_bool_literal(std::size_t& token_idx,
                                                        const Tokens& tokens);

  inline static const std::unordered_map<TokenType, PrefixFn> prefix_fns = {
      {TokenType::Identifier, parse_identifier},
      {TokenType::Int, parse_int_literal},
      {TokenType::Minus, parse_prefix_expression},
      {TokenType::Bang, parse_prefix_expression},
      {TokenType::Bool, parse_bool_literal},
  };

  // INFIX
  inline static const std::unordered_map<TokenType, InfixFn> infix_fns = {};
};

#endif  // !PARSER_HPP
