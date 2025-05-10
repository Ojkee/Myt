#include <memory>
#include <tuple>

#include "../extern/include/catch.hpp"
#include "model/data_cell.hpp"
#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/evaluator.hpp"
#include "model/myt_lang/lexer.hpp"
#include "model/myt_lang/myt_object.hpp"
#include "model/myt_lang/parser.hpp"
#include "model/page.hpp"

namespace Catch {
template <>
struct StringMaker<MytObject> {
  static std::string convert(MytObject const& value) {
    return "{" + value.to_string() + "}";
  }
};
}  // namespace Catch

TEST_CASE("Basic Value Objects") {
  using inputType = std::string;
  using targetType = MytObjectPtr;
  using testCases = std::vector<std::tuple<inputType, targetType, CellMap>>;

  testCases cases{};
  cases.emplace_back("= 5", std::make_unique<ValueObject<int>>(5), CellMap{});
  cases.emplace_back("= 6.9", std::make_unique<ValueObject<FloatType>>(6.9),
                     CellMap{});
  cases.emplace_back("= \"lmao\"",
                     std::make_unique<ValueObject<std::string>>("lmao"),
                     CellMap{});
  cases.emplace_back("= false", std::make_unique<ValueObject<bool>>(false),
                     CellMap{});
  cases.emplace_back(
      "= B22", std::make_unique<ValueObject<bool>>(true),
      CellMap{{CellPos{"B22"},
               DataCell{"= true", std::make_unique<ValueObject<bool>>(true)}}});

  for (const auto& [input, target, cells] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    const auto evaluated = Evaluator::evaluate(parsed, cells);
    CHECK(*evaluated == *target);
  }
}

TEST_CASE("Prefix Expressions to Objects") {
  using inputType = std::string;
  using targetType = MytObjectPtr;
  using testCases = std::vector<std::tuple<inputType, targetType, CellMap>>;

  testCases cases{};
  cases.emplace_back("= -5", std::make_unique<ValueObject<int>>(-5), CellMap{});
  cases.emplace_back("= -6.9", std::make_unique<ValueObject<FloatType>>(-6.9),
                     CellMap{});
  cases.emplace_back("= !true", std::make_unique<ValueObject<bool>>(false),
                     CellMap{});
  cases.emplace_back("= !false", std::make_unique<ValueObject<bool>>(true),
                     CellMap{});
  cases.emplace_back(
      "= !lmao", std::make_unique<ErrorObject>("Invalid prefix `!` argument"),
      CellMap{});
  cases.emplace_back(
      "= -lmao", std::make_unique<ErrorObject>("Invalid prefix `-` argument"),
      CellMap{});
  cases.emplace_back(
      "= -B3", std::make_unique<ValueObject<int>>(-5),
      CellMap{{CellPos{"B3"},
               DataCell{"= 5", std::make_unique<ValueObject<int>>(5)}}});
  cases.emplace_back(
      "= -ZZ3", std::make_unique<ValueObject<FloatType>>(-6.9),
      CellMap{
          {CellPos{"ZZ3"},
           DataCell{"= 6.9", std::make_unique<ValueObject<FloatType>>(6.9)}}});
  cases.emplace_back(
      "= !ZZ3", std::make_unique<ValueObject<bool>>(false),
      CellMap{{CellPos{"ZZ3"},
               DataCell{"= true", std::make_unique<ValueObject<bool>>(true)}}});

  for (const auto& [input, target, cells] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    const auto evaluated = Evaluator::evaluate(parsed, cells);
    CHECK(*evaluated == *target);
  }
}

#define ZERO_DIV_ERR std::make_shared<ErrorObject>("Can't divide by 0")

TEST_CASE("Infix Expressions to Objects") {
  using inputType = std::string;
  using targetType = MytObjectPtr;
  using testCases = std::vector<std::tuple<inputType, targetType, CellMap>>;

  testCases cases{};
  cases.emplace_back("= 1 + 6", std::make_unique<ValueObject<int>>(7),
                     CellMap{});
  cases.emplace_back("= 2.0 + 6.0",
                     std::make_unique<ValueObject<FloatType>>(8.0), CellMap{});
  cases.emplace_back("= 3.0 + 6", std::make_unique<ValueObject<FloatType>>(9.0),
                     CellMap{});
  cases.emplace_back("= 4 + 6.0",
                     std::make_unique<ValueObject<FloatType>>(10.0), CellMap{});
  cases.emplace_back("= \"lmao\" + \"6.0\"",
                     std::make_unique<ValueObject<std::string>>("lmao6.0"),
                     CellMap{});
  cases.emplace_back("= 1 - 6", std::make_unique<ValueObject<int>>(-5),
                     CellMap{});
  cases.emplace_back("= 2.0 - 6.0",
                     std::make_unique<ValueObject<FloatType>>(-4.0), CellMap{});
  cases.emplace_back("= 3.0 - 6",
                     std::make_unique<ValueObject<FloatType>>(-3.0), CellMap{});
  cases.emplace_back("= 4 - 6.0",
                     std::make_unique<ValueObject<FloatType>>(-2.0), CellMap{});
  cases.emplace_back("= 1 * 6", std::make_unique<ValueObject<int>>(6),
                     CellMap{});
  cases.emplace_back("= 2.0 * 6.0",
                     std::make_unique<ValueObject<FloatType>>(12.0), CellMap{});
  cases.emplace_back("= 3.0 * 6",
                     std::make_unique<ValueObject<FloatType>>(18.0), CellMap{});
  cases.emplace_back("= 4 * 6.0",
                     std::make_unique<ValueObject<FloatType>>(24.0), CellMap{});
  cases.emplace_back("= 1 / 6", std::make_unique<ValueObject<int>>(0),
                     CellMap{});
  cases.emplace_back("= 2.0 / 6.0",
                     std::make_unique<ValueObject<FloatType>>(2.0 / 6.0),
                     CellMap{});
  cases.emplace_back("= 3.0 / 6",
                     std::make_unique<ValueObject<FloatType>>(3.0 / 6.0),
                     CellMap{});
  cases.emplace_back("= 4 / 6.0",
                     std::make_unique<ValueObject<FloatType>>(4.0 / 6.0),
                     CellMap{});
  cases.emplace_back("= 4 / 6.0",
                     std::make_unique<ValueObject<FloatType>>(4.0 / 6.0),
                     CellMap{});
  cases.emplace_back("= 4 / 0", ZERO_DIV_ERR, CellMap{});
  cases.emplace_back("= 4.0 / 0.0", ZERO_DIV_ERR, CellMap{});
  cases.emplace_back(
      "= B2 * A8", std::make_unique<ValueObject<FloatType>>(40.0),
      CellMap{
          {CellPos("B2"),
           DataCell("= 8", std::make_unique<ValueObject<int>>(8))},
          {CellPos("A8"),
           DataCell("= 5.", std::make_unique<ValueObject<FloatType>>(5.0))},
      });

  for (const auto& [input, target, cells] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    const auto evaluated = Evaluator::evaluate(parsed, cells);
    CHECK(*evaluated == *target);
  }
}
