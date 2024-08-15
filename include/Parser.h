#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "Ast.h"
#include "Lexer.h"

namespace hlx{
    class Parser{
        Lexer *lexer;
        Token nextToken;
        bool inCompleteAST=false;

        void eatNextToken(){nextToken=lexer->getNextToken();}
        void synchronize(TokenKind kind);

    public:
        explicit Parser(Lexer &lexer)
        : lexer(&lexer),
          nextToken(lexer.getNextToken()){}
        std::unique_ptr<ReturnStmt> parseReturnStmt();
        std::unique_ptr<Stmt> parseStmt();
        std::unique_ptr<FunctionDecl> parseFunctionDecl();
        std::optional<Type> parseType();
        std::unique_ptr<Block> parseBlock();
        std::unique_ptr<Expr> parsePrimary();
        std::unique_ptr<Expr> parseExpr();
        std::unique_ptr<ParamDecl> parseParamDecl();
        std::unique_ptr<std::vector<std::unique_ptr<ParamDecl>>>
        parseParameterList();
        std::unique_ptr<std::vector<std::unique_ptr<Expr>>>
        parseArgumentList();
        std::pair<std::vector<std::unique_ptr<FunctionDecl>>,bool> parseSourceFile();
    };
}