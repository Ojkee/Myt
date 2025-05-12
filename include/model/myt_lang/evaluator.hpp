#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/cell_pos.hpp"
#include "model/myt_lang/myt_object.hpp"
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
  [[nodiscard]] static auto eval_infix_colon(const ExpressionCell& lhs_cell,
                                             const ExpressionCell& rhs_cell,
                                             const CellMap& cells) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto eval_fn_call(const ExpressionFnCall& expr_fn_call,
                                         const CellMap& cells) noexcept
      -> MytObjectPtr;

  [[nodiscard]] static auto generate_cell_range(const CellPos& begin,
                                                const CellPos& end) noexcept
      -> std::vector<CellPos> {
    std::vector<CellPos> ret_val{};
    for (CellLimitType i{begin.col}; i <= end.col; ++i) {
      for (CellLimitType j{begin.row}; j <= end.row; ++j) {
        const auto pos = CellPos{i, j};
        ret_val.emplace_back(pos);
      }
    }
    return ret_val;
  }

  [[nodiscard]] static auto fill_cell_range(
      const std::vector<CellPos> positions, const CellMap& cells) noexcept
      -> std::vector<MytObjectPtr> {
    std::vector<MytObjectPtr> cells_range{};
    for (const auto& pos : positions) {
      if (is_in_cells(pos, cells)) {
        const auto data_cell = cells.at(pos);
        const auto obj = data_cell.get_evaluated_content();
        cells_range.emplace_back(obj);
      } else {
        const auto nil_obj = MS_T(NilObject, );
        cells_range.emplace_back(nil_obj);
      }
    }
    return cells_range;
  };
};

#endif  // !EVALUATOR_HPP
