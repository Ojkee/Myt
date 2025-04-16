#include "../../../include/model/myt_lang/lexer.hpp"

#include <cctype>
#include <cstddef>
#include <functional>
#include <iterator>
#include <numeric>
#include <optional>
#include <string>
#include <utility>
#include <vector>

std::vector<Token> Lexer::tokenize(
    const std::string_view& raw_content) noexcept {
  std::vector<Token> tokens{};

  auto i = raw_content.begin();
  while (i < raw_content.end()) {
    if (is_whitespace(*i)) {
      i++;
      continue;
    }
    if (auto single_char_tok = Lexer::get_sign_token(raw_content, i)) {
      tokens.emplace_back(*single_char_tok);
    } else if (*i == '\"') {
      i++;
      if (auto str = Lexer::read_forward_if(
              raw_content, i, [](const char& c) { return c != '\"'; }))
        tokens.emplace_back(Token{TokenType::String, std::string(*str)});
    } else if (auto ident = Lexer::read_forward_if(raw_content, i,
                                                   Lexer::is_identifier_char)) {
      if (auto keywordType =
              Lexer::get_token_type_keyword(std::string(*ident))) {
        tokens.emplace_back(Token{*keywordType, std::string(*ident)});
      } else {
        tokens.emplace_back(Token{TokenType::Identifier, std::string(*ident)});
      }
    } else if (auto num =
                   Lexer::read_forward_if(raw_content, i, Lexer::is_numeric)) {
      tokens.emplace_back(Token{TokenType::Int, std::string(*num)});
    };

    i++;
  }

  tokens.emplace_back(Token{TokenType::EndOfCell, "EOC"});
  return tokens;
}

std::string Lexer::tokens_to_string(const std::vector<Token>& tokens) noexcept {
  if (tokens.empty()) return "No tokens";
  if (tokens.size() == 1) return std::string(tokens[0]);
  auto new_line_fold = [](std::string lhs, const Token& tok) {
    return std::move(lhs) + "\n" + std::string(tok);
  };
  return std::accumulate(std::next(tokens.begin()), tokens.end(),
                         std::string(tokens[0]), new_line_fold);
}

constexpr inline bool Lexer::is_whitespace(const char& c) noexcept {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

constexpr inline bool Lexer::is_identifier_char(const char& c) noexcept {
  return std::isalpha(c) || c == '_';
}

constexpr inline bool Lexer::is_numeric(const char& c) noexcept {
  return std::isdigit(c);
}

std::optional<Token> Lexer::get_sign_token(
    const std::string_view& content, std::string_view::iterator& cur) noexcept {
  switch (*cur) {
    case '=':
      if (cur + 1 != content.end() && *(cur + 1) == '=') {
        cur++;
        return Token{TokenType::Eq, "=="};
      }
      return Token{TokenType::Assign, "="};
    case '!':
      if (cur + 1 != content.end() && *(cur + 1) == '=') {
        cur++;
        return Token{TokenType::NotEq, "!="};
      }
      return Token{TokenType::Bang, "!"};
    case '+':
      return Token{TokenType::Plus, "+"};
    case '-':
      return Token{TokenType::Minus, "-"};
    case '*':
      return Token{TokenType::Asterisk, "*"};
    case '/':
      return Token{TokenType::Slash, "/"};
    case ',':
      return Token{TokenType::Comma, ","};
    case '(':
      return Token{TokenType::LParen, "("};
    case ')':
      return Token{TokenType::RParen, ")"};
    case '>':
      if (cur + 1 != content.end() && *(cur + 1) == '=') {
        cur++;
        return Token{TokenType::Ge, ">="};
      }
      return Token{TokenType::Gt, ">"};
    case '<':
      if (cur + 1 != content.end() && *(cur + 1) == '=') {
        cur++;
        return Token{TokenType::Le, "<="};
      }
      return Token{TokenType::Lt, "<"};
  }
  return std::nullopt;
}

std::optional<std::string_view> Lexer::read_forward_if(
    const std::string_view& content, std::string_view::iterator& curIt,
    const std::function<bool(const char&)>& cond) noexcept {
  auto start = curIt;
  std::size_t len{0};
  while (curIt != content.end() && cond(*curIt)) {
    len++;
    curIt++;
  }
  if (len == 0) return std::nullopt;
  return std::string_view{start, len};
}

bool Lexer::is_keyword(const std::string& ident) noexcept {
  return TokenUtils::keywordsLookup.find(ident) !=
         TokenUtils::keywordsLookup.end();
}

std::optional<TokenType> Lexer::get_token_type_keyword(
    const std::string& ident) noexcept {
  if (!is_keyword(ident)) return std::nullopt;
  return TokenUtils::keywordsLookup.at(ident);
}
