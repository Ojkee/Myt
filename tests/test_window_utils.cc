#include "../extern/include/catch.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "frontend/window_utils.hpp"

TEST_CASE("Converting number to letter index") {
  using inputType = CellLimitType;
  using targetType = std::string;
  using testCases = std::vector<std::tuple<inputType, targetType>>;
  testCases cases{
      {1, "A"},    {2, "B"},     {12, "L"},    {26, "Z"},
      {27, "AA"},  {28, "AB"},   {155, "EY"},  {183, "GA"},
      {702, "ZZ"}, {703, "AAA"}, {900, "AHP"},
  };

  WindowUtils wu{};
  for (const auto& [input, target] : cases) {
    const auto result = wu.col_idx_to_letter(input);
    const auto result_str = result.toStdString();
    CHECK(result_str == target);
  }
}
