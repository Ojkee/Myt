#include "../../../include/model/myt_lang/parser.hpp"

#include <iterator>
#include <memory>
#include <numeric>
#include <string>

ParsingResult Parser::parse(const Tokens& tokens) noexcept {
  if (tokens.size() == 0) {
    return std::make_unique<ExpressionLiteral<std::string>>("");
  }
  if (tokens[0].type != TokenType::Assign) {
    const auto concat_content = Parser::concat_token_literals(tokens);
    return std::make_unique<ExpressionLiteral<std::string>>(concat_content);
  }
  return ParsingError{"Err"};
}

const std::string Parser::concat_token_literals(const Tokens& tokens) noexcept {
  if (tokens.empty()) return "";
  if (tokens.size() == 1) return tokens[0].literal;
  auto concat_space_fold = [](std::string lhs, const Token& tok) {
    if (tok.type == TokenType::EndOfCell) return lhs;
    return std::move(lhs) + " " + tok.literal;
  };
  return std::accumulate(std::next(tokens.begin()), tokens.end(),
                         tokens[0].literal, concat_space_fold);
}

ExpressionPtr Parser::parse_identifier(Tokens::iterator& it) {
  return std::make_unique<ExpressionIdentifier>((*it).literal);
}
