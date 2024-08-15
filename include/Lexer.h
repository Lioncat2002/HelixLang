#pragma once
#include <unordered_map>
#include <string_view>
#include <optional>
#include "Utils.h"

namespace hlx{
    constexpr char singleCharTokens[]={'\0','(',')','{','}',':',';',','};
    enum class TokenKind:char{
        Unk=-128,
        Identifier,
        Number,
        KwFn,
        KwVoid,
        KwNumber,
        KwReturn,
        Eof=singleCharTokens[0],
        Lpar=singleCharTokens[1],
        Rpar=singleCharTokens[2],
        Lbrace=singleCharTokens[3],
        Rbrace=singleCharTokens[4],
        Colon=singleCharTokens[5],
        Semi=singleCharTokens[6],
        Comma=singleCharTokens[7],
    };

    const std::unordered_map<std::string_view,TokenKind> keywords={
            {"fn",TokenKind::KwFn},
            {"void",TokenKind::KwVoid},
            {"return",TokenKind::KwReturn},
            {"number",TokenKind::KwNumber}

    };

    struct Token{
        SourceLocation location;
        TokenKind kind;
        std::optional<std::string> value=std::nullopt;
    };

    class Lexer{
        const SourceFile *source;
        size_t idx=0;
        int line=1;
        int column=0;
    private:
        char peekNextChar() const;
        char eatNextChar();
    public:
        explicit Lexer(const SourceFile &source):source(&source){}
        Token getNextToken();
    };
}
