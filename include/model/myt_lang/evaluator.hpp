#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "model/data_cell.hpp"
#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/parser.hpp"
#include "model/page.hpp"

class Evaluator {
 public:
  [[nodiscard]] static auto evaluate(const ParsingResult& parsed_result,
                                     const CellMap& cells) noexcept
      -> MytObjectPtr;

 private:
  [[nodiscard]] static auto evaluate_expression(const Expression& expr,
                                                const CellMap& cells) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto is_in_cells(const CellPos& cell_pos,
                                        const CellMap& cells) noexcept -> bool {
    return cells.find(cell_pos) != cells.end();
  };
  [[nodiscard]] static auto get_from_cells(const ExpressionCell& expr_cell,
                                           const CellMap& cells) noexcept
      -> MytObjectPtr;

  [[nodiscard]] static auto eval_prefix(const ExpressionPrefix& expr_prefix,
                                        const CellMap& cells) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto eval_prefix_bang(MytObjectPtr obj) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto eval_prefix_minus(MytObjectPtr obj) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto eval_infix(const ExpressionInfix& exrp_infix,
                                       const CellMap& cells) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto eval_fn_call(const ExpressionFnCall& expr_fn_call,
                                         const CellMap& cells) noexcept
      -> MytObjectPtr;
};

#endif  // !EVALUATOR_HPP
