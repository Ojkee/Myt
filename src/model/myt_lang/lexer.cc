#include "../../../include/model/myt_lang/lexer.hpp"

#include <cctype>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// TODO: implement Floats
std::vector<Token> Lexer::tokenize(
    const std::string_view& raw_content) noexcept {
  const auto not_qoute = [](const char& c) { return c != '\"'; };

  std::vector<Token> tokens{};
  auto i = raw_content.begin();
  while (i < raw_content.end()) {
    if (is_whitespace(*i)) {
      i++;
      continue;
    } else if (auto single_char_tok = Lexer::get_sign_token(raw_content, i)) {
      tokens.emplace_back(*single_char_tok);
    } else if (*i == '\"') {
      i++;
      if (auto str = Lexer::read_forward_if(raw_content, i, not_qoute)) {
        tokens.emplace_back(Token{TokenType::String, std::string(*str)});
      }
    } else if (auto cell_ident = Lexer::read_cell_indent(raw_content, i)) {
      const auto token_value = std::string(*cell_ident);
      const auto token = Token{TokenType::CellIdentifier, token_value};
      tokens.emplace_back(token);
    } else if (auto ident = Lexer::read_forward_if(raw_content, i,
                                                   Lexer::is_identifier_char)) {
      if (auto keywordType =
              Lexer::get_token_type_keyword(std::string(*ident))) {
        const auto token = Token{*keywordType, std::string(*ident)};
        tokens.emplace_back(token);
      } else {
        const auto token = Token{TokenType::Identifier, std::string(*ident)};
        tokens.emplace_back(token);
      }
    } else if (auto num =
                   Lexer::read_forward_if(raw_content, i, Lexer::is_numeric)) {
      const auto token = Token{TokenType::Int, std::string(*num)};
      tokens.emplace_back(token);
    } else {
      const auto token = Token{TokenType::Illegal, "Illegal"};
      tokens.emplace_back(token);
    }

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

constexpr inline bool Lexer::is_cell_identifier_char(const char& c) noexcept {
  return std::isalpha(c) && std::isupper(c);
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
    const std::string_view& content, std::string_view::iterator& cur_it,
    const Condition& cond) noexcept {
  auto start = cur_it;
  std::size_t len{0};
  while (cur_it != content.end() && cond(*cur_it)) {
    len++;
    cur_it++;
  }
  return (len != 0) ? std::optional(std::string_view{start, len})
                    : std::nullopt;
}

std::optional<std::string_view> Lexer::read_cell_indent(
    const std::string_view& content, std::string_view::iterator& cur_it) {
  bool started_letter_idx{false}, started_number_idx{false};

  const auto start = cur_it;
  auto it = cur_it;
  while (it != content.end() && !Lexer::is_whitespace(*it)) {
    if (Lexer::is_cell_identifier_char(*it)) {
      if (started_number_idx)
        return std::nullopt;
      else if (!started_letter_idx)
        started_letter_idx = true;
      it++;
    } else if (Lexer::is_numeric(*it)) {
      if (!started_letter_idx)
        return std::nullopt;
      else if (!started_number_idx) {
        if (*it == '0') return std::nullopt;
        started_number_idx = true;
      }
      it++;
    } else {
      return std::nullopt;
    }
  }
  if (started_letter_idx && started_number_idx) {
    cur_it = it;
    auto len = static_cast<std::size_t>(it - start);
    return std::string_view{start, len};
  }
  return std::nullopt;
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
