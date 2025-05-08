#include <memory>
#include <tuple>

#include "../extern/include/catch.hpp"
#include "model/data_cell.hpp"
#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/evaluator.hpp"
#include "model/myt_lang/lexer.hpp"
#include "model/myt_lang/myt_object.hpp"
#include "model/myt_lang/parser.hpp"

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
  using cellsType = std::unordered_map<CellPos, DataCell>;
  using testCases = std::vector<std::tuple<inputType, targetType, cellsType>>;

  testCases cases{};
  cases.emplace_back("= 5", std::make_unique<ValueObject<int>>(5), cellsType{});
  cases.emplace_back("= 6.9", std::make_unique<ValueObject<FloatType>>(6.9),
                     cellsType{});
  cases.emplace_back("= \"lmao\"",
                     std::make_unique<ValueObject<std::string>>("lmao"),
                     cellsType{});
  cases.emplace_back("= false", std::make_unique<ValueObject<bool>>(false),
                     cellsType{});

  for (const auto& [input, target, cells] : cases) {
    const auto tokens = Lexer::tokenize(input);
    const auto parsed = Parser::parse(tokens);
    const auto evaluated = Evaluator::evaluate(parsed, cells);
    CHECK(*evaluated == *target);
  }
}
