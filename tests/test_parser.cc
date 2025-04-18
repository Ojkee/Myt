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
  using input = std::string;
  using output = std::unique_ptr<Expression>;
  using testCases = std::vector<std::tuple<input, output>>;

  testCases cases;
  cases.emplace_back(
      "some test",
      std::make_unique<ExpressionLiteral<std::string>>("some test"));
  cases.emplace_back("8 !",
                     std::make_unique<ExpressionLiteral<std::string>>("8 !"));
  cases.emplace_back("= 8", std::make_unique<ExpressionLiteral<int>>(8));

  for (const auto& testCase : cases) {
    auto tokens = Lexer::tokenize(std::get<0>(testCase));
    auto parsed = Parser::parse(tokens);
    if (std::holds_alternative<ParsingError>(parsed)) {
      auto err = std::get<ParsingError>(parsed);
      FAIL("Want Expression, got ParsingError: " + err.content + " in: `" +
           std::get<0>(testCase) + "`");
    }
    auto expr = &std::get<std::unique_ptr<Expression>>(parsed);
    if (expr == nullptr) {
      FAIL("Want Expression, got nullptr");
    }
    // REMEMBER TO FIX (ADDRES COMPERISON)
    CHECK(**expr == *std::get<1>(testCase));
  }
}

TEST_CASE("Parsing invalid inputs") {}
