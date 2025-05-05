#include <string>
#include <tuple>
#include <vector>

#include "../extern/include/catch.hpp"
#include "../include/model/myt_lang/lexer.hpp"
#include "../include/model/myt_lang/token.hpp"

namespace Catch {
template <>
struct StringMaker<std::vector<Token>> {
  static std::string convert(std::vector<Token> const& value) {
    return "{" + Lexer::tokens_to_string(value) + "}";
  }
};
}  // namespace Catch

TEST_CASE("Tokenization") {
  using testCases = std::vector<std::tuple<std::string, std::vector<Token>>>;
  testCases cases{{"= + ! - * / (,)",
                   {
                       Token{TokenType::Assign, "="},
                       Token{TokenType::Plus, "+"},
                       Token{TokenType::Bang, "!"},
                       Token{TokenType::Minus, "-"},
                       Token{TokenType::Asterisk, "*"},
                       Token{TokenType::Slash, "/"},
                       Token{TokenType::LParen, "("},
                       Token{TokenType::Comma, ","},
                       Token{TokenType::RParen, ")"},
                       Token{TokenType::EndOfCell, "EOC"},
                   }},
                  {"> < >= <= == !=",
                   {
                       Token{TokenType::Gt, ">"},
                       Token{TokenType::Lt, "<"},
                       Token{TokenType::Ge, ">="},
                       Token{TokenType::Le, "<="},
                       Token{TokenType::Eq, "=="},
                       Token{TokenType::NotEq, "!="},
                       Token{TokenType::EndOfCell, "EOC"},
                   }},
                  {"true false if some other_words",
                   {
                       Token{TokenType::Bool, "true"},
                       Token{TokenType::Bool, "false"},
                       Token{TokenType::If, "if"},
                       Token{TokenType::Identifier, "some"},
                       Token{TokenType::Identifier, "other_words"},
                       Token{TokenType::EndOfCell, "EOC"},
                   }},
                  {"5 88 -301 0",
                   {
                       Token{TokenType::Int, "5"},
                       Token{TokenType::Int, "88"},
                       Token{TokenType::Minus, "-"},
                       Token{TokenType::Int, "301"},
                       Token{TokenType::Int, "0"},
                       Token{TokenType::EndOfCell, "EOC"},
                   }},
                  {"\"Some text 88 ! <>\"  \"Other str =\"",
                   {
                       Token{TokenType::String, "Some text 88 ! <>"},
                       Token{TokenType::String, "Other str ="},
                       Token{TokenType::EndOfCell, "EOC"},
                   }},
                  {"\"Some text 88 ! <>\"  \"Other str =\"! 9 , \"!\"",
                   {
                       Token{TokenType::String, "Some text 88 ! <>"},
                       Token{TokenType::String, "Other str ="},
                       Token{TokenType::Bang, "!"},
                       Token{TokenType::Int, "9"},
                       Token{TokenType::Comma, ","},
                       Token{TokenType::String, "!"},
                       Token{TokenType::EndOfCell, "EOC"},
                   }},
                  {"C110 B30 A9 AZ30 ZK1 ZK01",
                   {
                       Token{TokenType::CellIdentifier, "C110"},
                       Token{TokenType::CellIdentifier, "B30"},
                       Token{TokenType::CellIdentifier, "A9"},
                       Token{TokenType::CellIdentifier, "AZ30"},
                       Token{TokenType::CellIdentifier, "ZK1"},
                       Token{TokenType::Identifier, "ZK"},
                       Token{TokenType::Int, "1"},
                       Token{TokenType::EndOfCell, "EOC"},
                   }}};

  for (const auto& [input, target] : cases) {
    CHECK(Lexer::tokenize(input) == target);
  }
}
