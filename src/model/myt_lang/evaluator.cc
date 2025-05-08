#include "../../../include/model/myt_lang/evaluator.hpp"

#include <memory>
#include <variant>

#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/myt_object.hpp"

MytObjectPtr Evaluator::evaluate(
    const ParsingResult& parsed_result,
    const std::unordered_map<CellPos, DataCell>& cells) noexcept {
  if (std::holds_alternative<ParsingError>(parsed_result)) {
    const auto err = &std::get<ParsingError>(parsed_result);
    return std::make_unique<ErrorObject>(err->content);
  }

  const auto expr = &std::get<ExpressionPtr>(parsed_result);
  if (auto exrp_int = dynamic_cast<ExpressionLiteral<int>*>(expr->get())) {
    return std::make_unique<ValueObject<int>>(exrp_int->get_value());
  } else if (auto exrp_int =
                 dynamic_cast<ExpressionLiteral<std::string>*>(expr->get())) {
    return std::make_unique<ValueObject<std::string>>(exrp_int->get_value());
  } else if (auto exrp_int =
                 dynamic_cast<ExpressionLiteral<bool>*>(expr->get())) {
    return std::make_unique<ValueObject<bool>>(exrp_int->get_value());
  } else if (auto exrp_int =
                 dynamic_cast<ExpressionLiteral<FloatType>*>(expr->get())) {
    return std::make_unique<ValueObject<FloatType>>(exrp_int->get_value());
  }

  return std::make_unique<NilObject>();
}
