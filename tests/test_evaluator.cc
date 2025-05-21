#include <memory>
#include <tuple>

#include "../extern/include/catch.hpp"
#include "backend/data_cell.hpp"
#include "backend/myt_lang/ast.hpp"
#include "backend/myt_lang/evaluator.hpp"
#include "backend/myt_lang/lexer.hpp"
#include "backend/myt_lang/myt_object.hpp"
#include "backend/myt_lang/parser.hpp"
#include "backend/page.hpp"

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
  cases.emplace_back(
      "= B2:B4",
      std::make_unique<CellRangeObject>(
          "B2:B4",
          std::vector<MytObjectPtr>{
              std::make_shared<ValueObject<int>>(4),
              std::make_shared<ValueObject<int>>(5),
              std::make_shared<ValueObject<int>>(6),
          }),
      CellMap{
          {CellPos("A2"),
           DataCell("= 1", std::make_shared<ValueObject<int>>(1))},
          {CellPos("A3"),
           DataCell("= 2", std::make_shared<ValueObject<int>>(2))},
          {CellPos("B1"),
           DataCell("= 3", std::make_shared<ValueObject<int>>(3))},
          {CellPos("B2"),
           DataCell("= 4", std::make_shared<ValueObject<int>>(4))},
          {CellPos("B3"),
           DataCell("= 5", std::make_shared<ValueObject<int>>(5))},
          {CellPos("B4"),
           DataCell("= 6", std::make_shared<ValueObject<int>>(6))},
          {CellPos("B5"),
           DataCell("= 7", std::make_shared<ValueObject<int>>(7))},
          {CellPos("C3"),
           DataCell("= 8", std::make_shared<ValueObject<int>>(8))},
      });
  cases.emplace_back(
      "= B2:D2",
      std::make_unique<CellRangeObject>(
          "B2:D2",
          std::vector<MytObjectPtr>{
              std::make_shared<ValueObject<int>>(4),
              std::make_shared<ValueObject<int>>(7),
              std::make_shared<ValueObject<int>>(8),
          }),
      CellMap{
          {CellPos("A2"),
           DataCell("= 1", std::make_shared<ValueObject<int>>(1))},
          {CellPos("A3"),
           DataCell("= 2", std::make_shared<ValueObject<int>>(2))},
          {CellPos("B1"),
           DataCell("= 3", std::make_shared<ValueObject<int>>(3))},
          {CellPos("B2"),
           DataCell("= 4", std::make_shared<ValueObject<int>>(4))},
          {CellPos("B3"),
           DataCell("= 5", std::make_shared<ValueObject<int>>(5))},
          {CellPos("B4"),
           DataCell("= 6", std::make_shared<ValueObject<int>>(6))},
          {CellPos("C2"),
           DataCell("= 7", std::make_shared<ValueObject<int>>(7))},
          {CellPos("D2"),
           DataCell("= 8", std::make_shared<ValueObject<int>>(8))},
          {CellPos("E2"),
           DataCell("= 9", std::make_shared<ValueObject<int>>(9))},
      });
  cases.emplace_back(
      "= B2:D2",
      std::make_unique<CellRangeObject>(
          "B2:D2",
          std::vector<MytObjectPtr>{
              std::make_shared<ValueObject<int>>(4),
              std::make_shared<NilObject>(),
              std::make_shared<ValueObject<int>>(8),
          }),
      CellMap{
          {CellPos("A2"),
           DataCell("= 1", std::make_shared<ValueObject<int>>(1))},
          {CellPos("A3"),
           DataCell("= 2", std::make_shared<ValueObject<int>>(2))},
          {CellPos("B1"),
           DataCell("= 3", std::make_shared<ValueObject<int>>(3))},
          {CellPos("B2"),
           DataCell("= 4", std::make_shared<ValueObject<int>>(4))},
          {CellPos("B3"),
           DataCell("= 5", std::make_shared<ValueObject<int>>(5))},
          {CellPos("B4"),
           DataCell("= 6", std::make_shared<ValueObject<int>>(6))},
          {CellPos("D2"),
           DataCell("= 8", std::make_shared<ValueObject<int>>(8))},
          {CellPos("E2"),
           DataCell("= 9", std::make_shared<ValueObject<int>>(9))},
      });
  cases.emplace_back(
      "= B1:C2",
      std::make_unique<CellRangeObject>(
          "B1:C2",
          std::vector<MytObjectPtr>{
              std::make_shared<ValueObject<int>>(3),
              std::make_shared<ValueObject<int>>(4),
              std::make_shared<NilObject>(),
              std::make_shared<ValueObject<int>>(7),
          }),
      CellMap{
          {CellPos("A2"),
           DataCell("= 1", std::make_shared<ValueObject<int>>(1))},
          {CellPos("A3"),
           DataCell("= 2", std::make_shared<ValueObject<int>>(2))},
          {CellPos("B1"),
           DataCell("= 3", std::make_shared<ValueObject<int>>(3))},
          {CellPos("B2"),
           DataCell("= 4", std::make_shared<ValueObject<int>>(4))},
          {CellPos("B3"),
           DataCell("= 5", std::make_shared<ValueObject<int>>(5))},
          {CellPos("B4"),
           DataCell("= 6", std::make_shared<ValueObject<int>>(6))},
          {CellPos("C2"),
           DataCell("= 7", std::make_shared<ValueObject<int>>(7))},
          {CellPos("D2"),
           DataCell("= 8", std::make_shared<ValueObject<int>>(8))},
          {CellPos("E2"),
           DataCell("= 9", std::make_shared<ValueObject<int>>(9))},
      });

  for (const auto& [input, target, cells] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    const auto evaluated = Evaluator::evaluate(parsed, cells);
    CHECK(*evaluated == *target);
  }
}

TEST_CASE("No Arg Call Expressions to Objects") {
  using inputType = std::string;
  using targetType = MytObjectPtr;
  using testCases = std::vector<std::tuple<inputType, targetType, CellMap>>;

  testCases cases{};
  cases.emplace_back(
      "= Pi()", std::make_unique<ValueObject<FloatType>>(3.141593), CellMap{});

  for (const auto& [input, target, cells] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    const auto evaluated = Evaluator::evaluate(parsed, cells);
    CHECK(*evaluated == *target);
  }
}

TEST_CASE("Single Arg Call Expressions to Objects") {
  using inputType = std::string;
  using targetType = MytObjectPtr;
  using testCases = std::vector<std::tuple<inputType, targetType, CellMap>>;

  testCases cases{};
  cases.emplace_back("= Sqrt(9)", std::make_unique<ValueObject<FloatType>>(3.0),
                     CellMap{});
  cases.emplace_back("= Sqrt(25.0)",
                     std::make_unique<ValueObject<FloatType>>(5.0), CellMap{});
  cases.emplace_back("= Sqrt(3)",
                     std::make_unique<ValueObject<FloatType>>(1.732051),
                     CellMap{});
  cases.emplace_back(
      "= Sqrt(A8)", std::make_unique<ValueObject<FloatType>>(4.0),
      CellMap{
          {CellPos("A8"),
           DataCell("= 16.", std::make_unique<ValueObject<FloatType>>(16.0))},
      });
  cases.emplace_back(
      "= Sqrt(A8+C9)", std::make_unique<ValueObject<FloatType>>(7.0),
      CellMap{
          {CellPos("A8"),
           DataCell("= 16.", std::make_unique<ValueObject<FloatType>>(16.0))},
          {CellPos("C9"),
           DataCell("= 33", std::make_unique<ValueObject<int>>(33))},
      });

  for (const auto& [input, target, cells] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    const auto evaluated = Evaluator::evaluate(parsed, cells);
    CHECK(*evaluated == *target);
  }
}

TEST_CASE("Multi Arg Call Expressions to Objects") {
  using inputType = std::string;
  using targetType = MytObjectPtr;
  using testCases = std::vector<std::tuple<inputType, targetType, CellMap>>;

  testCases cases{};
  cases.emplace_back("= Sum(3)", std::make_unique<ValueObject<int>>(3),
                     CellMap{});
  cases.emplace_back(
      "= Sum(B2:D4, 5, A1, (5*2), -1)", std::make_unique<ValueObject<int>>(97),
      CellMap{
          {CellPos("A1"),
           DataCell("= 2", std::make_unique<ValueObject<int>>(2))},
          {CellPos("A2"),
           DataCell("= 3", std::make_unique<ValueObject<int>>(3))},
          {CellPos("A3"),
           DataCell("= 4", std::make_unique<ValueObject<int>>(4))},
          {CellPos("B2"),
           DataCell("= 5", std::make_unique<ValueObject<int>>(5))},
          {CellPos("B3"),
           DataCell("= 6", std::make_unique<ValueObject<int>>(6))},
          {CellPos("B4"),
           DataCell("= 7", std::make_unique<ValueObject<int>>(7))},
          {CellPos("C2"),
           DataCell("= 8", std::make_unique<ValueObject<int>>(8))},
          {CellPos("C3"),
           DataCell("= 9", std::make_unique<ValueObject<int>>(9))},
          {CellPos("C4"),
           DataCell("= 10", std::make_unique<ValueObject<int>>(10))},
          {CellPos("D2"),
           DataCell("= 11", std::make_unique<ValueObject<int>>(11))},
          {CellPos("D3"),
           DataCell("= 12", std::make_unique<ValueObject<int>>(12))},
          {CellPos("D4"),
           DataCell("= 13", std::make_unique<ValueObject<int>>(13))},
      });
  cases.emplace_back(
      "= Sum(B2:D4, 5.0, A1, (5*2), -1)",
      std::make_unique<ValueObject<FloatType>>(97.0),
      CellMap{
          {CellPos("A1"),
           DataCell("= 2", std::make_unique<ValueObject<int>>(2))},
          {CellPos("A2"),
           DataCell("= 3", std::make_unique<ValueObject<int>>(3))},
          {CellPos("A3"),
           DataCell("= 4", std::make_unique<ValueObject<int>>(4))},
          {CellPos("B2"),
           DataCell("= 5", std::make_unique<ValueObject<int>>(5))},
          {CellPos("B3"),
           DataCell("= 6", std::make_unique<ValueObject<int>>(6))},
          {CellPos("B4"),
           DataCell("= 7", std::make_unique<ValueObject<int>>(7))},
          {CellPos("C2"),
           DataCell("= 8", std::make_unique<ValueObject<int>>(8))},
          {CellPos("C3"),
           DataCell("= 9", std::make_unique<ValueObject<int>>(9))},
          {CellPos("C4"),
           DataCell("= 10", std::make_unique<ValueObject<int>>(10))},
          {CellPos("D2"),
           DataCell("= 11", std::make_unique<ValueObject<int>>(11))},
          {CellPos("D3"),
           DataCell("= 12", std::make_unique<ValueObject<int>>(12))},
          {CellPos("D4"),
           DataCell("= 13", std::make_unique<ValueObject<int>>(13))},
      });
  cases.emplace_back(
      "= Sum(B2:D4, 5, A1, (5*2), -1)",
      std::make_unique<ValueObject<FloatType>>(97.0),
      CellMap{
          {CellPos("A1"),
           DataCell("= 2.0", std::make_unique<ValueObject<FloatType>>(2.0))},
          {CellPos("A2"),
           DataCell("= 3", std::make_unique<ValueObject<int>>(3))},
          {CellPos("A3"),
           DataCell("= 4", std::make_unique<ValueObject<int>>(4))},
          {CellPos("B2"),
           DataCell("= 5", std::make_unique<ValueObject<int>>(5))},
          {CellPos("B3"),
           DataCell("= 6", std::make_unique<ValueObject<int>>(6))},
          {CellPos("B4"),
           DataCell("= 7", std::make_unique<ValueObject<int>>(7))},
          {CellPos("C2"),
           DataCell("= 8", std::make_unique<ValueObject<int>>(8))},
          {CellPos("C3"),
           DataCell("= 9", std::make_unique<ValueObject<int>>(9))},
          {CellPos("C4"),
           DataCell("= 10", std::make_unique<ValueObject<int>>(10))},
          {CellPos("D2"),
           DataCell("= 11", std::make_unique<ValueObject<int>>(11))},
          {CellPos("D3"),
           DataCell("= 12", std::make_unique<ValueObject<int>>(12))},
          {CellPos("D4"),
           DataCell("= 13", std::make_unique<ValueObject<int>>(13))},
      });
  cases.emplace_back(
      "= Sum(B2:D4, 5, A1, (5*2), -1)",
      std::make_unique<ValueObject<FloatType>>(97.0),
      CellMap{
          {CellPos("A1"),
           DataCell("= 2", std::make_unique<ValueObject<int>>(2))},
          {CellPos("A2"),
           DataCell("= 3", std::make_unique<ValueObject<int>>(3))},
          {CellPos("A3"),
           DataCell("= 4", std::make_unique<ValueObject<int>>(4))},
          {CellPos("B2"),
           DataCell("= 5", std::make_unique<ValueObject<int>>(5))},
          {CellPos("B3"),
           DataCell("= 6", std::make_unique<ValueObject<int>>(6))},
          {CellPos("B4"),
           DataCell("= 7", std::make_unique<ValueObject<int>>(7))},
          {CellPos("C2"),
           DataCell("= 8", std::make_unique<ValueObject<int>>(8))},
          {CellPos("C3"),
           DataCell("= 9.0", std::make_unique<ValueObject<FloatType>>(9.0))},
          {CellPos("C4"),
           DataCell("= 10", std::make_unique<ValueObject<int>>(10))},
          {CellPos("D2"),
           DataCell("= 11", std::make_unique<ValueObject<int>>(11))},
          {CellPos("D3"),
           DataCell("= 12", std::make_unique<ValueObject<int>>(12))},
          {CellPos("D4"),
           DataCell("= 13", std::make_unique<ValueObject<int>>(13))},
      });

  for (const auto& [input, target, cells] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    const auto evaluated = Evaluator::evaluate(parsed, cells);
    CHECK(*evaluated == *target);
  }
}
