#include <string>
#include <tuple>
#include <vector>

#include "../extern/include/catch.hpp"
#include "backend/myt_lang/cell_pos.hpp"

namespace Catch {
template <>
struct StringMaker<CellPos> {
  static std::string convert(CellPos const& value) {
    return "{" + std::to_string(value.col) + ", " + std::to_string(value.row) +
           "}";
  }
};
}  // namespace Catch

TEST_CASE("Converting string cell pos to row/col pos") {
  using inputType = std::string;
  using targetType = CellPos;
  using testCasesType = std::vector<std::tuple<inputType, targetType>>;

  testCasesType cases{
      {"A1", CellPos{1, 1}},       {"B2", CellPos{2, 2}},
      {"Z55", CellPos{26, 55}},    {"AA255", CellPos{27, 255}},
      {"AB255", CellPos{28, 255}}, {"UI255", CellPos{255, 255}},
  };

  for (const auto& [input, target] : cases) {
    try {
      CHECK(CellPos(input) == target);
    } catch (const std::exception& e) {
      FAIL(e.what());
    }
  }
}
