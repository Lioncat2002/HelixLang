#include "Lexer.h"
#include "Token.h"

bool isSpace(char c) {
  return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
         c == '\v';
}

bool isAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}
bool isNum(char c) { return '0' <= c && c <= '9'; }
bool isAlnum(char c) { return isAlpha(c) || isNum(c); }

char hlx::Lexer::peekNextChar() const { return source->buffer[idx]; }

char hlx::Lexer::eatNextChar() {
  ++column;
  if (source->buffer[idx] == '\n') {
    ++line;
    column = 0;
  }
  return source->buffer[idx++];
}

hlx::Token hlx::Lexer::getNextToken() {
  char currentChar = eatNextChar();
  while (isSpace(currentChar)) {
    currentChar = eatNextChar();
  }
  SourceLocation tokenStartLocation{source->path, line, column};

  if (currentChar == '>' && peekNextChar() == '=') {
    eatNextChar();
    return Token{tokenStartLocation, TokenKind::MoreThanEql};
  }
  if (currentChar == '<' && peekNextChar() == '=') {
    eatNextChar();
    return Token{tokenStartLocation, TokenKind::LessThanEql};
  }

  if (currentChar == '!' && peekNextChar() == '=') {
    eatNextChar();
    return Token{tokenStartLocation, TokenKind::NotEqual};
  }

  if (currentChar == '=') {
    if(peekNextChar()!='='){
      return Token{tokenStartLocation,TokenKind::Equal};
    }
    eatNextChar();
    return Token{tokenStartLocation, TokenKind::EqualEqual};
  }

  if (currentChar == '&' && peekNextChar() == '&') {
    eatNextChar();
    return Token{tokenStartLocation, TokenKind::AmpAmp};
  }

  if (currentChar == '|' && peekNextChar() == '|') {
    eatNextChar();
    return Token{tokenStartLocation, TokenKind::PipePipe};
  }

  if (currentChar == '/') {
    if (peekNextChar() != '/')
      return Token{tokenStartLocation, TokenKind::Slash};
    while (peekNextChar() != '\n' && peekNextChar() != '\0')
      eatNextChar();

    return getNextToken();
  }

  for (auto &&c : singleCharTokens) {
    if (c == currentChar)
      return Token{tokenStartLocation, static_cast<TokenKind>(c)};
  }

  if (isAlpha(currentChar)) {
    std::string value{currentChar};
    while (isAlnum(peekNextChar()))
      value += eatNextChar();

    if (keywords.count(value))
      return Token{tokenStartLocation, keywords.at(value), std::move(value)};
    return Token{tokenStartLocation, TokenKind::Identifier, std::move(value)};
  }

  if (isNum(currentChar)) {
    std::string value{currentChar};
    while (isNum(peekNextChar()))
      value += eatNextChar();
    if (peekNextChar() != '.')
      return Token{tokenStartLocation, TokenKind::Number, value};
    value += eatNextChar();
    if (!isNum(peekNextChar()))
      return Token{tokenStartLocation, TokenKind::Unk};
    while (isNum(peekNextChar()))
      value += eatNextChar();
    return Token{tokenStartLocation, TokenKind::Number, value};
  }
  return Token{tokenStartLocation, TokenKind::Unk};
}
