#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <variant>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

using ExpressionSharedPtr = std::shared_ptr<Expression>;
using ParsingResult = std::variant<ExpressionSharedPtr, ParsingError>;
using ParsingUniqueResult = std::variant<ExpressionPtr, ParsingError>;
using ArgumentsResult = std::variant<Arguments, ParsingError>;
using Tokens = std::vector<Token>;
using PrefixFn = std::function<ParsingUniqueResult(std::size_t& token_idx,
                                                   const Tokens& tokens)>;
using InfixFn = std::function<ParsingUniqueResult(
    ExpressionPtr, std::size_t& token_idx, const Tokens& tokens)>;

class Parser {
 public:
  [[nodiscard]] static auto parse(const Tokens& tokens) noexcept
      -> ParsingResult;
  static auto print_result(const ParsingResult& result) noexcept -> void;

 private:
  template <class FnType>
  [[nodiscard]] static auto is_in_fns(
      const std::unordered_map<TokenType, FnType>& fns,
      const TokenType& type) noexcept -> bool {
    return fns.find(type) != fns.end();
  }
  [[nodiscard]] static auto parse_expression(
      std::size_t& token_idx, const Tokens& tokens,
      const Precendence&& precendence) noexcept -> ParsingUniqueResult;
  [[nodiscard]] static auto parse_prefix_fn(std::size_t& token_idx,
                                            const Tokens& tokens) noexcept
      -> ParsingUniqueResult;
  [[nodiscard]] static auto concat_token_literals(const std::size_t& start_idx,
                                                  const Tokens& tokens) noexcept
      -> const std::string;
  [[nodiscard]] static auto is_precendence_higher(
      const std::size_t& token_idx, const Tokens& tokens,
      const Precendence& precendence) noexcept -> bool;
  [[nodiscard]] static auto parse_call_arguments(std::size_t& token_idx,
                                                 const Tokens& tokens) noexcept
      -> ArgumentsResult;

  // PREFIX
  [[nodiscard]] static auto parse_prefix_expression(
      std::size_t& token_idx, const Tokens& tokens) noexcept
      -> ParsingUniqueResult;
  [[nodiscard]] static auto parse_identifier(std::size_t& token_idx,
                                             const Tokens& tokens) noexcept
      -> ExpressionPtr;
  [[nodiscard]] static auto parse_cell_identifier(std::size_t& token_idx,
                                                  const Tokens& tokens) noexcept
      -> ExpressionPtr;
  [[nodiscard]] static auto parse_int_literal(std::size_t& token_idx,
                                              const Tokens& tokens) noexcept
      -> ParsingUniqueResult;
  [[nodiscard]] static auto parse_float_literal(std::size_t& token_idx,
                                                const Tokens& tokens) noexcept
      -> ParsingUniqueResult;
  [[nodiscard]] static auto parse_bool_literal(std::size_t& token_idx,
                                               const Tokens& tokens) noexcept
      -> ParsingUniqueResult;
  [[nodiscard]] static auto parse_string_literal(std::size_t& token_idx,
                                                 const Tokens& tokens) noexcept
      -> ParsingUniqueResult;
  [[nodiscard]] static auto parse_grouped_expression(
      std::size_t& token_idx, const Tokens& tokens) noexcept
      -> ParsingUniqueResult;

  inline static const std::unordered_map<TokenType, PrefixFn> prefix_fns = {
      {TokenType::Minus, parse_prefix_expression},
      {TokenType::Bang, parse_prefix_expression},
      {TokenType::Identifier, parse_identifier},
      {TokenType::CellIdentifier, parse_cell_identifier},
      {TokenType::Int, parse_int_literal},
      {TokenType::Float, parse_float_literal},
      {TokenType::Bool, parse_bool_literal},
      {TokenType::String, parse_string_literal},
      {TokenType::LParen, parse_grouped_expression},
  };

  // INFIX
  template <typename InfixType>
  [[nodiscard]] static auto parse_infix_expression(
      ExpressionPtr lhs_expression, std::size_t& token_idx,
      const Tokens& tokens) noexcept -> ParsingUniqueResult;
  [[nodiscard]] static auto parse_fn_call_expression(
      ExpressionPtr lhs_expression, std::size_t& token_idx,
      const Tokens& tokens) noexcept -> ParsingUniqueResult;

  inline static const std::unordered_map<TokenType, InfixFn> infix_fns = {
      {TokenType::Plus, parse_infix_expression<ExpressionInfix>},
      {TokenType::Minus, parse_infix_expression<ExpressionInfix>},
      {TokenType::Asterisk, parse_infix_expression<ExpressionInfix>},
      {TokenType::Slash, parse_infix_expression<ExpressionInfix>},
      {TokenType::Colon, parse_infix_expression<ExpressionCellRange>},
      {TokenType::Eq, parse_infix_expression<ExpressionInfix>},
      {TokenType::NotEq, parse_infix_expression<ExpressionInfix>},
      {TokenType::Gt, parse_infix_expression<ExpressionInfix>},
      {TokenType::Ge, parse_infix_expression<ExpressionInfix>},
      {TokenType::Lt, parse_infix_expression<ExpressionInfix>},
      {TokenType::Le, parse_infix_expression<ExpressionInfix>},
      {TokenType::LParen, parse_fn_call_expression},
  };
};

#endif  // !PARSER_HPP
