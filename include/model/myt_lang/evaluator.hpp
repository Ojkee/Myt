#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "model/data_cell.hpp"
#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/parser.hpp"
#include "model/page.hpp"

class Evaluator {
 public:
  [[nodiscard]] static MytObjectPtr evaluate(const ParsingResult& parsed_result,
                                             const CellMap& cells) noexcept;

 private:
  [[nodiscard]] static MytObjectPtr evaluate_expression(
      const Expression& expr, const CellMap& cells) noexcept;
  [[nodiscard]] static bool is_in_cells(const CellPos& cell_pos,
                                        const CellMap& cells) noexcept {
    return cells.find(cell_pos) != cells.end();
  };
  [[nodiscard]] static MytObjectPtr get_from_cells(
      const ExpressionCell& expr_cell, const CellMap& cells) noexcept;

  // PREFIX
  [[nodiscard]] static MytObjectPtr eval_prefix(
      const ExpressionPrefix& expr_prefix, const CellMap& cells) noexcept;
  [[nodiscard]] static MytObjectPtr eval_prefix_bang(MytObjectPtr obj) noexcept;
  [[nodiscard]] static MytObjectPtr eval_prefix_minus(
      MytObjectPtr obj) noexcept;

  // INFIX
  [[nodiscard]] static MytObjectPtr eval_infix(
      const ExpressionInfix& exrp_infix, const CellMap& cells) noexcept;
};

#endif  // !EVALUATOR_HPP
