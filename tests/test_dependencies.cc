#include <qobject.h>

#include <string>
#include <tuple>
#include <vector>

#include "../extern/include/catch.hpp"
#include "backend/cell_dependencies_handler.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/state.hpp"

auto col_idx_to_letter_str(const CellLimitType& n) -> std::string {
  std::string acc = "";
  for (CellLimitType i = n; i > 0; i /= 26) {
    i -= 1;
    acc = (char)((i % 26) + 'A') + acc;
  }
  return acc;
}

namespace Catch {
template <>
struct StringMaker<Dependencies> {
  static std::string convert(Dependencies const& value) {
    const auto pos_to_str = [](const auto& pos) {
      return col_idx_to_letter_str(pos.col) + std::to_string(pos.row);
    };
    std::string acc{""};
    for (const auto& [pos, affected] : value) {
      auto line = "\t" + pos_to_str(pos) + ": ";
      for (const auto& affected_pos : affected) {
        line += pos_to_str(affected_pos) + " ";
      }
      acc += line + "\n";
    }
    return acc;
  }
};
}  // namespace Catch

TEST_CASE("Basic dependencies") {
  using cellInputs = std::vector<std::tuple<std::string, CellPos>>;
  using testCases = std::vector<std::tuple<cellInputs, Dependencies>>;

  testCases cases = {
      {
          cellInputs{
              {"=B2", CellPos{1, 1}},
              {"=B5", CellPos{1, 2}},
          },
          Dependencies{
              {CellPos{"B2"}, {CellPos{"A1"}}},
              {CellPos{"B5"}, {CellPos{"A2"}}},
          },
      },
      {
          cellInputs{
              {"=B2+B8", CellPos{1, 1}},
              {"=B5*B5+B8", CellPos{1, 2}},
          },
          Dependencies{
              {CellPos{"B2"}, {CellPos{"A1"}}},
              {CellPos{"B8"}, {CellPos{"A1"}, CellPos{"A2"}}},
              {CellPos{"B5"}, {CellPos{"A2"}}},
          },
      },
  };

  for (auto& [inputs, deps] : cases) {
    State state{};
    for (const auto& [input, pos] : inputs) {
      const auto q_input = QString::fromStdString(input);
      state.eval_save(q_input, pos.col, pos.row);
    }
    CHECK(state.get_dependencies() == deps);
  }
}
