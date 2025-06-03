#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <type_traits>

#include "backend/myt_lang/ast.hpp"
#include "backend/myt_lang/cell_pos.hpp"
#include "backend/myt_lang/myt_object.hpp"
#include "backend/myt_lang/parser.hpp"
#include "backend/page.hpp"

using ObjectsResult =
    std::variant<std::vector<MytObjectPtr>, std::shared_ptr<ErrorObject>>;

#define MS_VO_T(T, value) std::make_shared<ValueObject<T>>(value)
#define MS_T(T, value) std::make_shared<T>(value)
#define D_CAST(T, expr) dynamic_cast<const T*>(expr)
#define DP_CAST_T(T, value) std::dynamic_pointer_cast<T>(value)
#define DP_CAST_VO_T(T, value) std::dynamic_pointer_cast<ValueObject<T>>(value)
#define ZERO_DIV_ERR std::make_shared<ErrorObject>("Can't divide by 0")
#define NOT_IMPL_ERR(value)                                    \
  std::make_shared<ErrorObject>("Fn: `" + std::string(value) + \
                                "` not implemented")

class Evaluator {
 public:
  [[nodiscard]] static auto evaluate(const ParsingResult& parsed_result,
                                     const CellMap& cells) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto get_error_obj(const std::string& msg) noexcept
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
  [[nodiscard]] static auto eval_infix(const ExpressionInfix& expr_infix,
                                       const CellMap& cells) noexcept
      -> MytObjectPtr;
  [[nodiscard]] static auto eval_cell_range(
      const ExpressionCellRange& expr_cell_range,
      const CellMap& cells) noexcept -> MytObjectPtr;
  [[nodiscard]] static auto eval_fn_call(const ExpressionFnCall& expr_fn_call,
                                         const CellMap& cells) noexcept
      -> MytObjectPtr;

  [[nodiscard]] static auto generate_cell_range(const CellPos& begin,
                                                const CellPos& end) noexcept
      -> std::vector<CellPos> {
    std::vector<CellPos> ret_val{};
    for (auto i{begin.col}; i <= end.col; ++i) {
      for (auto j{begin.row}; j <= end.row; ++j) {
        const auto pos = CellPos{i, j};
        ret_val.emplace_back(pos);
      }
    }
    return ret_val;
  }

  [[nodiscard]] static auto fill_cell_range(
      const std::vector<CellPos> positions,
      const CellMap& cells) noexcept -> ObjectsResult {
    std::vector<MytObjectPtr> cells_range{};
    for (const auto& pos : positions) {
      if (is_in_cells(pos, cells)) {
        const auto data_cell = cells.at(pos);
        const auto obj = data_cell.get_evaluated_content();
        if (auto cell_range = DP_CAST_T(CellRangeObject, obj)) {
          const auto err_msg =
              "Invalid expression: nested cell ranges are not supported yet";
          return MS_T(ErrorObject, err_msg);
        }
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
