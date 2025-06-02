#include "../../../include/backend/myt_lang/evaluator.hpp"

#include <cassert>
#include <memory>
#include <variant>
#include <vector>

#include "backend/data_cell.hpp"
#include "backend/myt_lang/ast.hpp"
#include "backend/myt_lang/myt_builtins.hpp"
#include "backend/myt_lang/myt_object.hpp"
#include "backend/myt_lang/token.hpp"
#include "backend/page.hpp"

auto Evaluator::evaluate(const ParsingResult& parsed_result,
                         const CellMap& cells) noexcept -> MytObjectPtr {
  if (std::holds_alternative<ParsingError>(parsed_result)) {
    const auto err = &std::get<ParsingError>(parsed_result);
    return MS_T(ErrorObject, err->content);
  }

  const auto expr = std::get<ExpressionSharedPtr>(parsed_result).get();
  return Evaluator::evaluate_expression(*expr, cells);
}

auto Evaluator::evaluate_expression(const Expression& expr,
                                    const CellMap& cells) noexcept
    -> MytObjectPtr {
  if (const auto expr_int = D_CAST(ExpressionLiteral<int>, &expr)) {
    return MS_VO_T(int, expr_int->get_value());

  } else if (const auto expr_int =
                 D_CAST(ExpressionLiteral<std::string>, &expr)) {
    return MS_VO_T(std::string, expr_int->get_value());

  } else if (const auto expr_int = D_CAST(ExpressionLiteral<bool>, &expr)) {
    return MS_VO_T(bool, expr_int->get_value());

  } else if (const auto expr_int =
                 D_CAST(ExpressionLiteral<FloatType>, &expr)) {
    return MS_VO_T(FloatType, expr_int->get_value());

  } else if (const auto expr_ident = D_CAST(ExpressionIdentifier, &expr)) {
    return MS_T(IdentObject, expr_ident->get_name_token().literal);

  } else if (const auto expr_cell = D_CAST(ExpressionCell, &expr)) {
    return Evaluator::get_from_cells(*expr_cell, cells);

  } else if (const auto expr_prefix = D_CAST(ExpressionPrefix, &expr)) {
    return Evaluator::eval_prefix(*expr_prefix, cells);

  } else if (const auto expr_cell_range = D_CAST(ExpressionCellRange, &expr)) {
    return Evaluator::eval_cell_range(*expr_cell_range, cells);

  } else if (const auto expr_infix = D_CAST(ExpressionInfix, &expr)) {
    return Evaluator::eval_infix(*expr_infix, cells);

  } else if (const auto expr_fn_call = D_CAST(ExpressionFnCall, &expr)) {
    return Evaluator::eval_fn_call(*expr_fn_call, cells);
  }

  return MS_T(NilObject, );
};

auto Evaluator::get_from_cells(const ExpressionCell& expr_cell,
                               const CellMap& cells) noexcept -> MytObjectPtr {
  const auto cell_str = expr_cell.get_cell_token().literal;
  const auto cell_pos = CellPos{cell_str};
  if (!Evaluator::is_in_cells(cell_pos, cells)) {
    return MS_T(NilObject, );
  }
  const auto data_cell = cells.at(cell_pos);
  return data_cell.get_evaluated_content();
}

auto Evaluator::eval_prefix(const ExpressionPrefix& expr_prefix,
                            const CellMap& cells) noexcept -> MytObjectPtr {
  const auto& expr_ref = expr_prefix.get_expression();
  const auto obj = Evaluator::evaluate_expression(expr_ref, cells);
  const auto prefix_type = expr_prefix.get_prefix_token().type;
  switch (prefix_type) {
    case TokenType::Bang:
      return Evaluator::eval_prefix_bang(obj);
    case TokenType::Minus:
      return Evaluator::eval_prefix_minus(obj);
    default:
      assert(false && "Unreachable");
  }
  return MS_T(NilObject, );
}

auto Evaluator::eval_prefix_bang(MytObjectPtr obj) noexcept -> MytObjectPtr {
  if (auto bool_obj = std::dynamic_pointer_cast<ValueObject<bool>>(obj)) {
    return MS_VO_T(bool, !bool_obj->get_value());
  }
  return MS_T(ErrorObject, "Invalid prefix `!` argument");
}

auto Evaluator::eval_prefix_minus(MytObjectPtr obj) noexcept -> MytObjectPtr {
  if (const auto int_obj = DP_CAST_VO_T(int, obj)) {
    return MS_VO_T(int, -int_obj->get_value());
  } else if (const auto float_obj = DP_CAST_VO_T(FloatType, obj)) {
    return MS_VO_T(FloatType, -float_obj->get_value());
  }
  return MS_T(ErrorObject, "Invalid prefix `-` argument");
}

auto Evaluator::eval_infix(const ExpressionInfix& expr_infix,
                           const CellMap& cells) noexcept -> MytObjectPtr {
  const auto& expr_lhs = expr_infix.get_lhs_expression();
  const auto& expr_rhs = expr_infix.get_rhs_expression();

  const auto lhs_obj = Evaluator::evaluate_expression(expr_lhs, cells);
  const auto rhs_obj = Evaluator::evaluate_expression(expr_rhs, cells);

  const auto op_token = expr_infix.get_operator_token();
  switch (op_token.type) {
    case TokenType::Plus:
      return lhs_obj->add(rhs_obj);
    case TokenType::Minus:
      return lhs_obj->sub(rhs_obj);
    case TokenType::Asterisk:
      return lhs_obj->mul(rhs_obj);
    case TokenType::Slash:
      if (const auto float_obj = DP_CAST_VO_T(FloatType, rhs_obj)) {
        if (float_obj.get()->get_value() == 0)
          return ZERO_DIV_ERR;
      } else if (const auto int_obj = DP_CAST_VO_T(int, rhs_obj)) {
        if (int_obj.get()->get_value() == 0)
          return ZERO_DIV_ERR;
      }
      return lhs_obj->div(rhs_obj);
    default:
      return MS_T(ErrorObject, "Unimplemented operator " + op_token.literal);
  }
  return MS_T(NilObject, );
}

auto Evaluator::eval_cell_range(const ExpressionCellRange& expr_cell_range,
                                const CellMap& cells) noexcept -> MytObjectPtr {
  const auto& expr_lhs = expr_cell_range.get_lhs_expression();
  const auto& expr_rhs = expr_cell_range.get_rhs_expression();

  auto lhs_cell = D_CAST(ExpressionCell, &expr_lhs);
  auto rhs_cell = D_CAST(ExpressionCell, &expr_rhs);
  if (lhs_cell == nullptr || rhs_cell == nullptr) {
    return MS_T(ErrorObject,
                "Wrong type, cell range requires "
                "`CellRow:CellCol`");
  }
  const auto range_positions = expr_cell_range.generate_range();
  const auto cells_result = Evaluator::fill_cell_range(range_positions, cells);

  if (std::holds_alternative<std::shared_ptr<ErrorObject>>(cells_result)) {
    const auto& err = std::get<std::shared_ptr<ErrorObject>>(cells_result);
    return err;
  }
  const auto [lhs_str, rhs_str] = expr_cell_range.get_cells_strs();
  const auto range_str = lhs_str + ":" + rhs_str;
  const auto cells_range = &std::get<std::vector<MytObjectPtr>>(cells_result);
  return std::make_shared<CellRangeObject>(range_str, *cells_range);
}

auto Evaluator::eval_fn_call(const ExpressionFnCall& expr_fn_call,
                             const CellMap& cells) noexcept -> MytObjectPtr {
  const auto& ident_expr = expr_fn_call.get_fn_identifier();
  const auto& args_expr = expr_fn_call.get_arguments();

  const auto ident_obj = Evaluator::evaluate_expression(ident_expr, cells);
  if (auto err_obj = DP_CAST_T(ErrorObject, ident_obj)) {
    return err_obj;
  }

  std::vector<MytObjectPtr> args;
  for (const auto& arg_expr : args_expr) {
    args.emplace_back(Evaluator::evaluate_expression(*arg_expr, cells));
  }

  if (auto casted_ident_obj = DP_CAST_T(IdentObject, ident_obj)) {
    const auto value = casted_ident_obj->get_value();
    return MytBuiltins::exec(value, args);
  }

  return NOT_IMPL_ERR("Wrong function identifier type: `" +
                      ident_obj->to_string() + "`");
}
