// #include "../include/app.hpp"

#include "model/myt_lang/lexer.hpp"
#include "model/myt_lang/parser.hpp"
int main() {
  // MytApp app;
  // app.run();
  const auto toks = Lexer::tokenize("= add(B2*2, 3)");
  const auto parsed = Parser::parse(toks);
  Parser::print_result(parsed);

  return 0;
}
