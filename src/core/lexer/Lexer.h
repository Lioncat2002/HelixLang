#pragma once
#include "../../utils/Utils.h"
#include "Token.h"


namespace hlx {

class Lexer {
  const SourceFile *source;
  size_t idx = 0;
  int line = 1;
  int column = 0;

private:
  char peekNextChar() const;
  char eatNextChar();

public:
  explicit Lexer(const SourceFile &source) : source(&source) {}
  Token getNextToken();
};
} // namespace hlx
