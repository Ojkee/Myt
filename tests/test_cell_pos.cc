#include <string>
#include <tuple>
#include <vector>

#include "../extern/include/catch.hpp"
#include "model/data_cell.hpp"

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
      {"Z55", CellPos{26, 55}},    {"AA256", CellPos{27, 256}},
      {"AB256", CellPos{28, 256}}, {"ZZ256", CellPos{702, 256}},
  };

  for (const auto& [input, target] : cases) {
    try {
      CHECK(CellPos(input) == target);
    } catch (const std::exception& e) {
      FAIL(e.what());
    }
  }
}
