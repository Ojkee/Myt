#include "../../../include/model/myt_lang/evaluator.hpp"

#include <cassert>
#include <memory>
#include <variant>

#include "model/data_cell.hpp"
#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/myt_object.hpp"
#include "model/myt_lang/token.hpp"
#include "model/page.hpp"

MytObjectPtr Evaluator::evaluate(const ParsingResult& parsed_result,
                                 const CellMap& cells) noexcept {
  if (std::holds_alternative<ParsingError>(parsed_result)) {
    const auto err = &std::get<ParsingError>(parsed_result);
    return std::make_unique<ErrorObject>(err->content);
  }

  const auto expr = std::get<ExpressionPtr>(parsed_result).get();
  return Evaluator::evaluate_expression(*expr, cells);
}

MytObjectPtr Evaluator::evaluate_expression(const Expression& expr,
                                            const CellMap& cells) noexcept {
  if (auto exrp_int = dynamic_cast<const ExpressionLiteral<int>*>(&expr)) {
    return std::make_shared<ValueObject<int>>(exrp_int->get_value());
  } else if (auto exrp_int =
                 dynamic_cast<const ExpressionLiteral<std::string>*>(&expr)) {
    return std::make_shared<ValueObject<std::string>>(exrp_int->get_value());
  } else if (auto exrp_int =
                 dynamic_cast<const ExpressionLiteral<bool>*>(&expr)) {
    return std::make_shared<ValueObject<bool>>(exrp_int->get_value());
  } else if (auto exrp_int =
                 dynamic_cast<const ExpressionLiteral<FloatType>*>(&expr)) {
    return std::make_shared<ValueObject<FloatType>>(exrp_int->get_value());
  } else if (auto expr_cell = dynamic_cast<const ExpressionCell*>(&expr)) {
    return Evaluator::get_from_cells(*expr_cell, cells);
  } else if (auto expr_prefix = dynamic_cast<const ExpressionPrefix*>(&expr)) {
    return Evaluator::eval_prefix(*expr_prefix, cells);
  }

  return std::make_shared<NilObject>();
};

MytObjectPtr Evaluator::get_from_cells(const ExpressionCell& expr_cell,
                                       const CellMap& cells) noexcept {
  const auto cell_str = expr_cell.get_cell_token().literal;
  const auto cell_pos = CellPos{cell_str};
  if (!Evaluator::is_in_cells(cell_pos, cells)) {
    return std::make_shared<NilObject>();
  }
  const auto data_cell = cells.at(cell_pos);
  return data_cell.get_evaluated_content();
}

MytObjectPtr Evaluator::eval_prefix(const ExpressionPrefix& expr_prefix,
                                    const CellMap& cells) noexcept {
  const auto& expr_ref = expr_prefix.get_expression();
  const auto obj = Evaluator::evaluate_expression(expr_ref, cells);
  const auto prefix_type = expr_prefix.get_prefix_token().type;
  switch (prefix_type) {
    case TokenType::Bang:
    case TokenType::Minus:
      break;
    default:
      assert(false && "Unreachable");
  }
  return std::make_shared<NilObject>();
}
