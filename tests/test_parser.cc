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
  using targetType = std::unique_ptr<Expression>;
  using testCases = std::vector<std::tuple<inputType, targetType>>;

  testCases cases;
  cases.emplace_back(
      "some test",
      std::make_unique<ExpressionLiteral<std::string>>("some test"));
  cases.emplace_back("8 !",
                     std::make_unique<ExpressionLiteral<std::string>>("8 !"));
  cases.emplace_back("= 8", std::make_unique<ExpressionLiteral<int>>(8));

  for (const auto& [input, target] : cases) {
    auto tokens = Lexer::tokenize(input);
    auto parsed = Parser::parse(tokens);
    if (std::holds_alternative<ParsingError>(parsed)) {
      auto err = std::get<ParsingError>(parsed);
      FAIL("Want Expression, got ParsingError: " + err.content + " in: `" +
           input + "`");
    }
    auto expr = &std::get<std::unique_ptr<Expression>>(parsed);
    if (expr == nullptr) {
      FAIL("Want Expression, got nullptr");
    }
    CHECK(**expr == *target);
  }
}

TEST_CASE("Parsing invalid inputs") {}
