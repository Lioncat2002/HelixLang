#pragma once
#include "../../utils/Utils.h"
#include <optional>
#include <unordered_map>
namespace hlx {
constexpr char singleCharTokens[] = {'\0', '(', ')', '{', '}', ':',
                                     ';',  ',', '+', '-', '*', '/'};
enum class TokenKind : char {
  Unk = -128,
  Identifier,
  Number,
  KwFn,
  KwVoid,
  KwNumber,
  KwReturn,
  Eof = singleCharTokens[0],
  Lpar = singleCharTokens[1],
  Rpar = singleCharTokens[2],
  Lbrace = singleCharTokens[3],
  Rbrace = singleCharTokens[4],
  Colon = singleCharTokens[5],
  Semi = singleCharTokens[6],
  Comma = singleCharTokens[7],
  Plus = singleCharTokens[8],
  Minus = singleCharTokens[9],
  Asterisk = singleCharTokens[10],
  Slash = singleCharTokens[11]
};

struct Token {
  SourceLocation location;
  TokenKind kind;
  std::optional<std::string> value = std::nullopt;
};

const std::unordered_map<std::string_view, TokenKind> keywords = {
    {"fn", TokenKind::KwFn},
    {"void", TokenKind::KwVoid},
    {"return", TokenKind::KwReturn},
    {"number", TokenKind::KwNumber}

};

} // namespace hlx