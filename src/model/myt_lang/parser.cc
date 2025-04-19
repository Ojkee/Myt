#include "../../../include/model/myt_lang/parser.hpp"

#include <climits>
#include <cstddef>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <variant>

ParsingResult Parser::parse(const Tokens& tokens) noexcept {
  if (tokens.size() == 0) {
    return std::make_unique<ExpressionLiteral<std::string>>("");
  }
  if (tokens[0].type != TokenType::Assign) {
    const auto concat_content = Parser::concat_token_literals(0, tokens);
    return std::make_unique<ExpressionLiteral<std::string>>(concat_content);
  }
  std::size_t currentIdx{1};
  auto expr_result = parse_expression(currentIdx, tokens);
  if (tokens.at(currentIdx).type != TokenType::EndOfCell) {
    const auto rest_concat = Parser::concat_token_literals(currentIdx, tokens);
    return ParsingError{"Couldn't parse: `" + rest_concat + "`"};
  }
  return expr_result;
}

ParsingResult Parser::parse_expression(std::size_t& tokenIdx,
                                       const Tokens& tokens) noexcept {
  auto prefixExpr = parse_prefix_expression(tokenIdx, tokens);
  if (std::holds_alternative<ParsingError>(prefixExpr)) {
    return prefixExpr;
  }
  // TODO: infix
  return prefixExpr;
}

ParsingResult Parser::parse_prefix_expression(std::size_t& tokenIdx,
                                              const Tokens& tokens) noexcept {
  if (is_in_fns(prefixFns, tokens.at(tokenIdx).type)) {
    const auto currentToken = tokens.at(tokenIdx);
    const auto prefixFn = prefixFns.at(currentToken.type);
    return prefixFn(tokenIdx, tokens);
  }
  return ParsingError{"Parse prefix expression not implemented"};
}

ParsingResult Parser::parse_infix_expression(std::size_t& tokenIdx,
                                             const Tokens& tokens) noexcept {
  return ParsingError{"Parse infix expression not implemented"};
}

const std::string Parser::concat_token_literals(const std::size_t& start,
                                                const Tokens& tokens) noexcept {
  if (tokens.empty() || (start >= tokens.size()) ||
      (tokens.size() == 1 && tokens[0].type == TokenType::EndOfCell)) {
    return "";
  }
  auto concat_space_fold = [](const std::string& lhs, const Token& tok) {
    if (tok.type == TokenType::EndOfCell) return lhs;
    return std::move(lhs) + " " + tok.literal;
  };
  auto begin = tokens.begin() + static_cast<long>(start + 1);
  return std::accumulate(begin, tokens.end(), tokens[start].literal,
                         concat_space_fold);
}

ExpressionPtr Parser::parse_identifier(std::size_t& tokenIdx,
                                       const Tokens& tokens) {
  auto ident =
      std::make_unique<ExpressionIdentifier>(tokens.at(tokenIdx).literal);
  tokenIdx++;
  return ident;
}

ParsingResult Parser::parse_int_literal(std::size_t& tokenIdx,
                                        const Tokens& tokens) {
  const auto currentToken = tokens.at(tokenIdx);
  tokenIdx++;
  try {
    const auto tokenValue = std::stoi(currentToken.literal);
    return std::make_unique<ExpressionLiteral<int>>(tokenValue);
  } catch (std::invalid_argument) {
    return ParsingError{"Couldn't parse: `" + currentToken.literal +
                        "` to int"};
  } catch (std::out_of_range) {
    return ParsingError{"Couldn't parse: `" + currentToken.literal +
                        "` to int, target range: (" + std::to_string(INT_MIN) +
                        ", " + std::to_string(INT_MAX) + ")"};
  }
}
