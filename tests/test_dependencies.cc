#include <qobject.h>

#include <string>
#include <tuple>
#include <vector>

#include "../extern/include/catch.hpp"
#include "backend/cell_dependencies_handler.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/state.hpp"

using Dependencies = DependenciesHandler::Dependencies;

namespace Catch {
template <>
struct StringMaker<Dependencies> {
  static std::string convert(Dependencies const& value) {
    std::string acc{""};
    for (const auto& [pos, affected] : value) {
      auto line = "\t" + pos.to_string() + ": ";
      for (const auto& affected_pos : affected) {
        line += affected_pos.to_string() + " ";
      }
      acc += line + "\n";
    }
    return acc;
  }
};
}  // namespace Catch

TEST_CASE("Dependencies insertions") {
  using cellInputs = std::vector<std::tuple<std::string, CellPos>>;
  using DepsAffected = Dependencies;
  using DepsUses = Dependencies;
  using testCases = std::vector<std::tuple<cellInputs, DepsAffected, DepsUses>>;

  testCases cases = {
      {
          cellInputs{
              {"=B2", CellPos{"A1"}},
              {"=B5", CellPos{"A2"}},
              {"=B3", CellPos{"A3"}},
          },
          Dependencies{
              {CellPos{"B2"}, {CellPos{"A1"}}},
              {CellPos{"B5"}, {CellPos{"A2"}}},
              {CellPos{"B3"}, {CellPos{"A3"}}},
          },
          Dependencies{
              {CellPos{"A1"}, {CellPos{"B2"}}},
              {CellPos{"A2"}, {CellPos{"B5"}}},
              {CellPos{"A3"}, {CellPos{"B3"}}},
          },
      },
      {
          cellInputs{
              {"=B2+B8", CellPos{"A1"}},
              {"=B5*B5+B8", CellPos{"A2"}},
              {"=B5*(B5+B8)/4-B5", CellPos{"A3"}},
          },
          Dependencies{
              {CellPos{"B2"}, {CellPos{"A1"}}},
              {CellPos{"B8"}, {CellPos{"A1"}, CellPos{"A2"}, CellPos{"A3"}}},
              {CellPos{"B5"}, {CellPos{"A2"}, CellPos{"A3"}}},
          },
          Dependencies{
              {CellPos{"A1"}, {CellPos{"B2"}, CellPos{"B8"}}},
              {CellPos{"A2"}, {CellPos{"B5"}, CellPos{"B8"}}},
              {CellPos{"A3"}, {CellPos{"B5"}, CellPos{"B8"}}},
          },
      },
      {
          cellInputs{
              {"=Sum(B1:B3, B5)", CellPos{"A1"}},
          },
          Dependencies{
              {CellPos{"B1"}, {CellPos{"A1"}}},
              {CellPos{"B2"}, {CellPos{"A1"}}},
              {CellPos{"B3"}, {CellPos{"A1"}}},
              {CellPos{"B5"}, {CellPos{"A1"}}},
          },
          Dependencies{
              {CellPos{"A1"},
               {CellPos{"B1"}, CellPos{"B2"}, CellPos{"B3"}, CellPos{"B5"}}},
          },
      },
  };

  for (auto& [inputs, deps_affected, deps_uses] : cases) {
    State state{};
    for (const auto& [input, pos] : inputs) {
      const auto q_input = QString::fromStdString(input);
      state.eval_save(q_input, pos.col, pos.row);
    }
    CHECK(state.get_dependencies() == deps_affected);
    CHECK(state.get_dependencies_uses() == deps_uses);
  }
}

TEST_CASE("Dependencies updates") {
  using cellInputs = std::vector<std::tuple<std::string, CellPos>>;
  using DepsAffected = Dependencies;
  using DepsUses = Dependencies;
  using testCases = std::vector<std::tuple<cellInputs, DepsAffected, DepsUses>>;

  testCases cases = {
      {
          cellInputs{
              {"=B2", CellPos{"A1"}},
              {"=B5", CellPos{"A2"}},
              {"=B3", CellPos{"A3"}},
              {"=B3", CellPos{"A1"}},
          },
          Dependencies{
              {CellPos{"B5"}, {CellPos{"A2"}}},
              {CellPos{"B3"}, {CellPos{"A1"}, CellPos{"A3"}}},
          },
          Dependencies{
              {CellPos{"A1"}, {CellPos{"B3"}}},
              {CellPos{"A2"}, {CellPos{"B5"}}},
              {CellPos{"A3"}, {CellPos{"B3"}}},
          },
      },
      {
          cellInputs{
              {"=B2+B8", CellPos{"A1"}},
              {"=B5*B5+B8", CellPos{"A2"}},
              {"=B5*(B5+B8)/4-B5", CellPos{"A1"}},
              {"", CellPos{"A2"}},
          },
          Dependencies{
              {CellPos{"B8"}, {CellPos{"A1"}}},
              {CellPos{"B5"}, {CellPos{"A1"}}},
          },
          Dependencies{
              {CellPos{"A1"}, {CellPos{"B5"}, CellPos{"B8"}}},
          },
      },
      {
          cellInputs{
              {"=Sum(B1:B3, B5)", CellPos{"A1"}},
              {"=Sum(B2:B4, B6)", CellPos{"A1"}},
          },
          Dependencies{
              {CellPos{"B2"}, {CellPos{"A1"}}},
              {CellPos{"B3"}, {CellPos{"A1"}}},
              {CellPos{"B4"}, {CellPos{"A1"}}},
              {CellPos{"B6"}, {CellPos{"A1"}}},
          },
          Dependencies{
              {CellPos{"A1"},
               {CellPos{"B2"}, CellPos{"B3"}, CellPos{"B4"}, CellPos{"B6"}}},
          },
      },
      {
          cellInputs{
              {"= Sum(A2:C4)", CellPos{"A1"}},
              {"= Sum(\"text that makes error\")", CellPos{"A1"}},
          },
          Dependencies{},
          Dependencies{},
      },
      {
          cellInputs{
              {"= Sum(A2:C4)", CellPos{"A1"}},
              {"= Sum(`)", CellPos{"A1"}},  // ` makes parsing error
          },
          Dependencies{},
          Dependencies{},
      },
  };

  for (auto& [inputs, deps_affected, deps_uses] : cases) {
    State state{};
    for (const auto& [input, pos] : inputs) {
      const auto q_input = QString::fromStdString(input);
      state.eval_save(q_input, pos.col, pos.row);
    }
    CHECK(state.get_dependencies() == deps_affected);
    CHECK(state.get_dependencies_uses() == deps_uses);
  }
}

TEST_CASE("Dependencies cycle detection") {
  using cellInputs = std::vector<std::tuple<std::string, CellPos>>;
  using DepsAffected = Dependencies;
  using DepsUses = Dependencies;
  using testCases = std::vector<std::tuple<cellInputs, DepsAffected, DepsUses>>;

  testCases cases = {
      {
          cellInputs{
              {"=A1", CellPos{"A1"}},
          },
          Dependencies{},
          Dependencies{},
      },
      {
          cellInputs{
              {"=A2", CellPos{"A1"}},
              {"=A3", CellPos{"A2"}},
              {"=A1", CellPos{"A3"}},
              {"=A4", CellPos{"A1"}},
          },
          Dependencies{
              {CellPos{"A4"}, {CellPos{"A1"}}},
          },
          Dependencies{
              {CellPos{"A1"}, {CellPos{"A4"}}},
          },
      },
  };

  for (auto& [inputs, deps_affected, deps_uses] : cases) {
    State state{};
    for (const auto& [input, pos] : inputs) {
      const auto q_input = QString::fromStdString(input);
      state.eval_save(q_input, pos.col, pos.row);
    }
    CHECK(state.get_dependencies() == deps_affected);
    CHECK(state.get_dependencies_uses() == deps_uses);
  }
}
