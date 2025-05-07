#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "model/myt_lang/ast.hpp"
#include "model/myt_lang/myt_object.hpp"

class Evaluator {
 public:
  [[nodiscard]] static MytObject evaluate(const ExpressionPtr&& expr) noexcept;

 private:
};

#endif  // !EVALUATOR_HPP
