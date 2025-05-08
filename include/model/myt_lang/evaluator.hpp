#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "model/data_cell.hpp"
#include "model/myt_lang/parser.hpp"

class Evaluator {
 public:
  [[nodiscard]] static MytObjectPtr evaluate(
      const ParsingResult& parsed_result,
      const std::unordered_map<CellPos, DataCell>& cells) noexcept;

 private:
};

#endif  // !EVALUATOR_HPP
