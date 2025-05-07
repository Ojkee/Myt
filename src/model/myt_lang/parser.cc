#include "../../../include/model/myt_lang/parser.hpp"

#include <climits>
#include <cstddef>
#include <iostream>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <variant>

#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/token.hpp"

void Parser::print_result(const ParsingResult& result) noexcept {
  if (std::holds_alternative<ParsingError>(result)) {
    const auto err = std::get<ParsingError>(result);
    std::cout << err.content << "\n";
    return;
  }
  const auto expr = &std::get<ExpressionPtr>(result);
  std::cout << expr->get()->to_string() << "\n";
}

ParsingResult Parser::parse(const Tokens& tokens) noexcept {
  if (tokens.size() == 0) {
    return std::make_unique<ExpressionLiteral<std::string>>("");
  }
  if (tokens[0].type != TokenType::Assign) {
    const auto concat_content = Parser::concat_token_literals(0, tokens);
    return std::make_unique<ExpressionLiteral<std::string>>(concat_content);
  }
  std::size_t current_idx{1};
  auto expr_result =
      Parser::parse_expression(current_idx, tokens, Precendence::Lowest);
  if (tokens.at(current_idx).type != TokenType::EndOfCell) {
    const auto rest_concat = Parser::concat_token_literals(current_idx, tokens);
    return ParsingError{"Couldn't parse: `" + rest_concat + "`"};
  }
  return expr_result;
}

ParsingResult Parser::parse_expression(
    std::size_t& token_idx, const Tokens& tokens,
    const Precendence&& precendence) noexcept {
  auto lhs_result = Parser::parse_prefix_fn(token_idx, tokens);
  if (std::holds_alternative<ParsingError>(lhs_result)) {
    return lhs_result;
  }

  while (tokens.at(token_idx).type != TokenType::EndOfCell &&
         Parser::is_precendence_higher(token_idx, tokens, precendence)) {
    if (!is_in_fns(infix_fns, tokens.at(token_idx).type)) {
      return lhs_result;
    }
    const auto next_token_type = tokens.at(token_idx).type;
    const auto infix_fn = infix_fns.at(next_token_type);
    const auto lhs_expression = &std::get<ExpressionPtr>(lhs_result);
    lhs_result = infix_fn(std::move(*lhs_expression), token_idx, tokens);
    if (std::holds_alternative<ParsingError>(lhs_result)) {
      return lhs_result;
    }
  }
  return lhs_result;
}

ParsingResult Parser::parse_prefix_fn(std::size_t& token_idx,
                                      const Tokens& tokens) noexcept {
  const auto current_token = tokens.at(token_idx);
  if (Parser::is_in_fns(prefix_fns, current_token.type)) {
    const auto prefixFn = prefix_fns.at(current_token.type);
    return prefixFn(token_idx, tokens);
  }
  return ParsingError{"Prefix expression for: `" + current_token.literal +
                      "` not implemented"};
}

const std::string Parser::concat_token_literals(const std::size_t& start_idx,
                                                const Tokens& tokens) noexcept {
  if (tokens.empty() || (start_idx >= tokens.size()) ||
      (tokens.size() == 1 && tokens[0].type == TokenType::EndOfCell)) {
    return "";
  }
  auto concat_space_fold = [](const std::string& lhs, const Token& tok) {
    if (tok.type == TokenType::EndOfCell) return lhs;
    return std::move(lhs) + " " + tok.literal;
  };
  auto begin = tokens.begin() + static_cast<long>(start_idx + 1);
  return std::accumulate(begin, tokens.end(), tokens[start_idx].literal,
                         concat_space_fold);
}

bool Parser::is_precendence_higher(const std::size_t& token_idx,
                                   const Tokens& tokens,
                                   const Precendence& precendence) noexcept {
  const auto next_token_type = tokens.at(token_idx).type;
  const auto peek_precendence = AstUtils::token_to_precendece(next_token_type);
  return precendence < peek_precendence;
}

ArgumentsResult Parser::parse_call_arguments(std::size_t& token_idx,
                                             const Tokens& tokens) noexcept {
  Arguments args{};
  if (tokens.at(token_idx).type == TokenType::RParen) {
    token_idx++;
    return args;
  }

  auto arg_result =
      Parser::parse_expression(token_idx, tokens, Precendence::Lowest);
  if (std::holds_alternative<ParsingError>(arg_result)) {
    return std::get<ParsingError>(arg_result);
  }
  auto& arg = std::get<ExpressionPtr>(arg_result);
  args.emplace_back(std::move(arg));

  while (token_idx < tokens.size() &&
         tokens.at(token_idx).type == TokenType::Comma) {
    token_idx++;
    auto arg_result =
        Parser::parse_expression(token_idx, tokens, Precendence::Lowest);
    if (std::holds_alternative<ParsingError>(arg_result)) {
      return std::get<ParsingError>(arg_result);
    }
    auto& arg = std::get<ExpressionPtr>(arg_result);
    args.emplace_back(std::move(arg));
  }

  if (token_idx < tokens.size() &&
      tokens.at(token_idx).type != TokenType::RParen) {
    return ParsingError{"Parsing arguments error, Expected: `)`, Got: " +
                        tokens.at(token_idx).literal + "`"};
  }
  token_idx++;
  return args;
}

// PREFIX
ParsingResult Parser::parse_prefix_expression(std::size_t& token_idx,
                                              const Tokens& tokens) noexcept {
  const auto prefix_token = tokens.at(token_idx++);
  auto rhs_result =
      Parser::parse_expression(token_idx, tokens, Precendence::Prefix);
  if (std::holds_alternative<ParsingError>(rhs_result)) {
    return rhs_result;
  }
  const auto rhs_expression = &std::get<ExpressionPtr>(rhs_result);
  return std::make_unique<ExpressionPrefix>(prefix_token,
                                            std::move(*rhs_expression));
}

ExpressionPtr Parser::parse_identifier(std::size_t& token_idx,
                                       const Tokens& tokens) noexcept {
  const auto token = tokens.at(token_idx++);
  return std::make_unique<ExpressionIdentifier>(token);
}

ExpressionPtr Parser::parse_cell_identifier(std::size_t& token_idx,
                                            const Tokens& tokens) noexcept {
  const auto cell_token = tokens.at(token_idx++);
  return std::make_unique<ExpressionCell>(cell_token);
}

ParsingResult Parser::parse_int_literal(std::size_t& token_idx,
                                        const Tokens& tokens) noexcept {
  const auto current_token = tokens.at(token_idx++);
  try {
    const auto tokenValue = std::stoi(current_token.literal);
    return std::make_unique<ExpressionLiteral<int>>(tokenValue);
  } catch (std::invalid_argument) {
    return ParsingError{"Couldn't parse: `" + current_token.literal +
                        "` to int"};
  } catch (std::out_of_range) {
    return ParsingError{"Couldn't parse: `" + current_token.literal +
                        "` to int, target range: (" + std::to_string(INT_MIN) +
                        ", " + std::to_string(INT_MAX) + ")"};
  }
}

ParsingResult Parser::parse_bool_literal(std::size_t& token_idx,
                                         const Tokens& tokens) noexcept {
  const auto token_value = tokens.at(token_idx++).literal == "true";
  return std::make_unique<ExpressionLiteral<bool>>(token_value);
}

ParsingResult Parser::parse_grouped_expression(std::size_t& token_idx,
                                               const Tokens& tokens) noexcept {
  auto expr =
      Parser::parse_expression(++token_idx, tokens, Precendence::Lowest);
  if (tokens.at(token_idx).type != TokenType::RParen) {
    return ParsingError{"expected: `)`, got: `" + tokens.at(token_idx).literal +
                        "`"};
  }
  token_idx++;
  return expr;
}

// INFIX
ParsingResult Parser::parse_infix_expression(ExpressionPtr lhs_expression,
                                             std::size_t& token_idx,
                                             const Tokens& tokens) noexcept {
  const auto operator_token = tokens.at(token_idx++);
  const auto precendence = AstUtils::token_to_precendece(operator_token.type);
  auto rhs_result =
      Parser::parse_expression(token_idx, tokens, std::move(precendence));
  if (std::holds_alternative<ParsingError>(rhs_result)) {
    return rhs_result;
  }
  const auto rhs_expression = &std::get<ExpressionPtr>(rhs_result);
  return std::make_unique<ExpressionInfix>(
      std::move(lhs_expression), operator_token, std::move(*rhs_expression));
}

ParsingResult Parser::parse_fn_call_expression(ExpressionPtr lhs_expression,
                                               std::size_t& token_idx,
                                               const Tokens& tokens) noexcept {
  auto args_result = Parser::parse_call_arguments(++token_idx, tokens);
  if (std::holds_alternative<ParsingError>(args_result)) {
    return std::get<ParsingError>(args_result);
  }
  auto& args = std::get<Arguments>(args_result);
  return std::make_unique<ExpressionFnCall>(std::move(lhs_expression),
                                            std::move(args));
}
