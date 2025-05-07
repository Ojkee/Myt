#include <memory>
#include <string>
#include <variant>

#include "../extern/include/catch.hpp"
#include "../include/model/myt_lang/ast.hpp"
#include "../include/model/myt_lang/lexer.hpp"
#include "../include/model/myt_lang/parser.hpp"

namespace Catch {
template <>
struct StringMaker<Expression> {
  static std::string convert(Expression const& value) {
    return "{" + value.to_string() + "}";
  }
};
}  // namespace Catch

TEST_CASE("Parsing valid inputs") {
  using inputType = std::string;
  using targetType = ExpressionPtr;
  using testCases = std::vector<std::tuple<inputType, targetType>>;

  testCases cases;
  cases.emplace_back(
      "some test",
      std::make_unique<ExpressionLiteral<std::string>>("some test"));
  cases.emplace_back("8 !",
                     std::make_unique<ExpressionLiteral<std::string>>("8 !"));
  cases.emplace_back("= 8", std::make_unique<ExpressionLiteral<int>>(8));
  cases.emplace_back("= -8", std::make_unique<ExpressionPrefix>(
                                 Token{TokenType::Minus, "-"},
                                 std::make_unique<ExpressionLiteral<int>>(8)));
  cases.emplace_back("= !true",
                     std::make_unique<ExpressionPrefix>(
                         Token{TokenType::Bang, "!"},
                         std::make_unique<ExpressionLiteral<bool>>(true)));
  cases.emplace_back("= 5 + 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Plus, "+"},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 - 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Minus, "-"},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 * 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Asterisk, "*"},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 / 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Slash, "/"},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 > 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Gt, ">"},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 < 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Lt, "<"},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 >= 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Ge, ">="},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 <= 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Le, "<="},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 == 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::Eq, "=="},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= 5 != 6",
                     std::make_unique<ExpressionInfix>(
                         std::make_unique<ExpressionLiteral<int>>(5),
                         Token{TokenType::NotEq, "!="},
                         std::make_unique<ExpressionLiteral<int>>(6)));
  cases.emplace_back("= A5:B3", std::make_unique<ExpressionInfix>(
                                    std::make_unique<ExpressionCell>(
                                        Token{TokenType::CellIdentifier, "A5"}),
                                    Token{TokenType::Colon, ":"},
                                    std::make_unique<ExpressionCell>(Token{
                                        TokenType::CellIdentifier, "B3"})));

  for (const auto& [input, target] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    if (std::holds_alternative<ParsingError>(parsed)) {
      const auto err = std::get<ParsingError>(parsed);
      FAIL("Want Expression, got ParsingError:\n\t" + err.content + " in: `" +
           input + "`");
    }
    const auto expr = &std::get<std::unique_ptr<Expression>>(parsed);
    if (expr == nullptr) {
      FAIL("Want Expression, got nullptr");
    }
    CHECK(*expr->get() == *target);
  }
}

TEST_CASE("Parsing precendence inputs") {
  using inputType = std::string;
  using targetType = std::string;
  using testCases = std::vector<std::tuple<inputType, targetType>>;

  testCases cases{
      {"= 5", "int(5)"},
      {"= a", "ident(a)"},
      {"5", "string(5)"},
      {"= 5 * 10", "infix(int(5)*int(10))"},
      {"", "string()"},
      {"= -1", "prefix(-int(1))"},
      {"= -a", "prefix(-ident(a))"},
      {"=C110", "cell(C110)"},
      {"= a + b - c", "infix(infix(ident(a)+ident(b))-ident(c))"},
      {"= a * b / c", "infix(infix(ident(a)*ident(b))/ident(c))"},
      {"= a + b * c", "infix(ident(a)+infix(ident(b)*ident(c)))"},
      {"= a + b * c + d / e - f",
       "infix(infix(infix(ident(a)+infix(ident(b)*ident(c)))+"
       "infix(ident(d)/ident(e)))-ident(f))"},
      {"= 3 + 4 * 5 == 3 * 1 + 4 * 5",
       "infix(infix(int(3)+infix(int(4)*int(5)))==infix(infix(int(3)*int(1))+"
       "infix(int(4)*int(5))))"},
      {"= B3 + A4 * C5 == B3 * D1 + K4 * AC5",
       "infix(infix(cell(B3)+infix(cell(A4)*cell(C5)))==infix(infix(cell(B3)*"
       "cell(D1))+"
       "infix(cell(K4)*cell(AC5))))"},
  };
  for (const auto& [input, target] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    if (std::holds_alternative<ParsingError>(parsed)) {
      const auto err = std::get<ParsingError>(parsed);
      FAIL("Want Expression, got ParsingError:\n\t" + err.content + " in: `" +
           input + "`");
    }
    const auto expr = &std::get<std::unique_ptr<Expression>>(parsed);
    if (expr == nullptr) {
      FAIL("Want Expression, got nullptr");
    }
    CHECK(expr->get()->to_string() == target);
  }
}

TEST_CASE("Parsing precendence inputs grouped") {
  using inputType = std::string;
  using targetType = std::string;
  using testCases = std::vector<std::tuple<inputType, targetType>>;

  testCases cases{
      {"= (5 + 6) * B1", "infix(infix(int(5)+int(6))*cell(B1))"},
      {"= !(true == B8)", "prefix(!infix(bool(true)==cell(B8)))"},
  };
  for (const auto& [input, target] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    if (std::holds_alternative<ParsingError>(parsed)) {
      const auto err = std::get<ParsingError>(parsed);
      FAIL("Want Expression, got ParsingError:\n\t" + err.content + " in: `" +
           input + "`");
    }
    const auto expr = &std::get<std::unique_ptr<Expression>>(parsed);
    if (expr == nullptr) {
      FAIL("Want Expression, got nullptr");
    }
    CHECK(expr->get()->to_string() == target);
  }
}

TEST_CASE("Parsing call expressions") {
  using inputType = std::string;
  using targetType = std::string;
  using testCases = std::vector<std::tuple<inputType, targetType>>;

  testCases cases{
      {"= foo()", "fn_call(ident(foo)())"},
      {"= bar(B23)", "fn_call(ident(bar)(cell(B23)))"},
      {"= add(5, 3)", "fn_call(ident(add)(int(5), int(3)))"},
      {"= add(5, sub(3, 2), 8)",
       "fn_call(ident(add)(int(5), fn_call(ident(sub)(int(3), int(2))), "
       "int(8)))"},
  };
  for (const auto& [input, target] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    if (std::holds_alternative<ParsingError>(parsed)) {
      const auto err = std::get<ParsingError>(parsed);
      FAIL("Want Expression, got ParsingError:\n\t" + err.content + " in: `" +
           input + "`");
    }
    const auto expr = &std::get<std::unique_ptr<Expression>>(parsed);
    if (expr == nullptr) {
      FAIL("Want Expression, got nullptr");
    }
    CHECK(expr->get()->to_string() == target);
  }
}

TEST_CASE("Parsing float expressions") {
  using inputType = std::string;
  using targetType = ExpressionPtr;
  using testCases = std::vector<std::tuple<inputType, targetType>>;

  testCases cases{};
  cases.emplace_back("= 55.0",
                     std::make_unique<ExpressionLiteral<FloatType>>(55.0));
  cases.emplace_back("= .03",
                     std::make_unique<ExpressionLiteral<FloatType>>(0.03));
  cases.emplace_back("= 93.53",
                     std::make_unique<ExpressionLiteral<FloatType>>(93.53));
  cases.emplace_back("= 028.890",
                     std::make_unique<ExpressionLiteral<FloatType>>(28.89));
  cases.emplace_back("= 34.",
                     std::make_unique<ExpressionLiteral<FloatType>>(34.0));

  for (const auto& [input, target] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    if (std::holds_alternative<ParsingError>(parsed)) {
      const auto err = std::get<ParsingError>(parsed);
      FAIL("Want Expression, got ParsingError:\n\t" + err.content + " in: `" +
           input + "`");
    }
    const auto expr = &std::get<std::unique_ptr<Expression>>(parsed);
    if (expr == nullptr) {
      FAIL("Want Expression, got nullptr");
    }
    CHECK(*expr->get() == *target);
  }
}
