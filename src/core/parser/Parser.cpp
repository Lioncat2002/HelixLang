#include "Parser.h"
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

void hlx::Parser::synchronize(hlx::TokenKind kind) {
  inCompleteAST = true;

  int braces = 0;
  while (true) {
    TokenKind kind = nextToken.kind;

    if (kind == TokenKind::Lbrace) {
      ++braces;
    } else if (kind == TokenKind::Rbrace) {
      if (braces == 0)
        break;

      if (braces == 1) {
        eatNextToken(); // eat '}'
        break;
      }

      --braces;
    } else if (kind == TokenKind::Semi && braces == 0) {
      eatNextToken(); // eat ';'
      break;
    } else if (kind == TokenKind::KwFn || kind == TokenKind::Eof)
      break;

    eatNextToken();
  }
}

std::pair<std::vector<std::unique_ptr<hlx::FunctionDecl>>, bool>
hlx::Parser::parseSourceFile() {
  std::vector<std::unique_ptr<FunctionDecl>> functions;

  while (nextToken.kind != TokenKind::Eof) {
    if (nextToken.kind != TokenKind::KwFn) {
      std::cerr<<(nextToken.kind==TokenKind::Rbrace);
      report(nextToken.location,
             "only function definitions are allowed on the top level");
      synchronize(TokenKind::KwFn);
      break;
    }

    auto fn = parseFunctionDecl();
    if (!fn) {
      synchronize(TokenKind::KwFn);
      continue;
    }

    functions.emplace_back(std::move(fn));
  }

  return {std::move(functions), !inCompleteAST};
}
//<functionDecl>
//::= 'fn' <ident> '(' ')' ':' <type> <block>
std::unique_ptr<hlx::FunctionDecl> hlx::Parser::parseFunctionDecl() {
  SourceLocation location = nextToken.location;
  eatNextToken();
  matchOrReturn(TokenKind::Identifier, "expected identifier");
  std::string functionIdentifier = *nextToken.value;
  eatNextToken();

  varOrReturn(parameterList, parseParameterList());

  matchOrReturn(TokenKind::Colon, "expected ':'");
  eatNextToken(); // eat ':'

  varOrReturn(type, parseType());

  matchOrReturn(TokenKind::Lbrace, "expected function body");
  varOrReturn(block, parseBlock());

  return std::make_unique<FunctionDecl>(location, functionIdentifier, *type,
                                        std::move(block),
                                        std::move(*parameterList));
}

std::optional<hlx::Type> hlx::Parser::parseType() {
  TokenKind kind = nextToken.kind;
  if (kind == TokenKind::KwVoid) {
    eatNextToken();
    return Type::builtinVoid();
  }

  if (kind == TokenKind::Number || kind == TokenKind::KwNumber) {
    eatNextToken();
    return Type::builtinNumber();
  }
  if (kind == TokenKind::Identifier) {
    auto t = Type::custom(*nextToken.value);
    eatNextToken();
    return t;
  }
  report(nextToken.location, "expected type specifier");
  return std::nullopt;
}

std::unique_ptr<hlx::Block> hlx::Parser::parseBlock() {
  SourceLocation location = nextToken.location;
  eatNextToken(); // eat '{'

  std::vector<std::unique_ptr<Stmt>> statements;
  while (true) {
    if (nextToken.kind == TokenKind::Rbrace)
      break;

    if (nextToken.kind == TokenKind::Eof || nextToken.kind == TokenKind::KwFn) {
      return report(nextToken.location, "expected '}' at the end of the block");
    }

    varOrReturn(stmt, parseStmt());
    statements.emplace_back(std::move(stmt));
  }
  matchOrReturn(TokenKind::Rbrace, "expected '}' at the end of a block");
  eatNextToken(); // eat '}'

  return std::make_unique<Block>(location, std::move(statements));
}

std::unique_ptr<hlx::ReturnStmt> hlx::Parser::parseReturnStmt() {
  SourceLocation location = nextToken.location;
  eatNextToken(); // eat return
  std::unique_ptr<Expr> expr;
  if (nextToken.kind != TokenKind::Semi) {
    expr = parseExpr();
    if (!expr)
      return nullptr;
  }
  matchOrReturn(TokenKind::Semi,
                "expected ';' at the end of a return statement");
  eatNextToken();
  return std::make_unique<ReturnStmt>(location, std::move(expr));
}

std::unique_ptr<hlx::IfStmt> hlx::Parser::parseIfStmt() {
  SourceLocation location = nextToken.location;
  eatNextToken(); // eat if

  varOrReturn(condition, parseExpr());

  matchOrReturn(TokenKind::Lbrace, "expected if body");

  varOrReturn(trueBlock, parseBlock());
  if (nextToken.kind != TokenKind::KwElse)
    return std::make_unique<IfStmt>(location, std::move(condition),
                                    std::move(trueBlock));

  eatNextToken(); // eat else
  std::unique_ptr<Block> falseBlock;
  if (nextToken.kind == TokenKind::KwIf) {
    varOrReturn(elseIf, parseIfStmt());
    SourceLocation loc = elseIf->location;
    std::vector<std::unique_ptr<Stmt>> stmts;
    stmts.emplace_back(std::move(elseIf));

    falseBlock = std::make_unique<Block>(loc, std::move(stmts));
  } else {
    matchOrReturn(TokenKind::Lbrace, "expected else body");
    falseBlock = parseBlock();
  }
  if (!falseBlock)
    return nullptr;

  return std::make_unique<IfStmt>(location, std::move(condition),
                                  std::move(trueBlock), std::move(falseBlock));
}

std::unique_ptr<hlx::WhileStmt> hlx::Parser::parseWhileStmt(){
  SourceLocation location=nextToken.location;
  eatNextToken();

  varOrReturn(cond, parseExpr());

  matchOrReturn(TokenKind::Lbrace,"expected 'while' body");

  varOrReturn(body, parseBlock());

  return std::make_unique<WhileStmt>(location,std::move(cond),std::move(body));
}

std::unique_ptr<hlx::Stmt> hlx::Parser::parseStmt() {
  if (nextToken.kind == TokenKind::KwIf)
    return parseIfStmt();
  if(nextToken.kind==TokenKind::KwWhile)
    return parseWhileStmt();
  if (nextToken.kind == TokenKind::KwReturn)
    return parseReturnStmt();
  if(nextToken.kind==TokenKind::KwLet || nextToken.kind==TokenKind::KwVar)
    return parseDeclStmt();
  varOrReturn(expr, parseExpr());
  matchOrReturn(TokenKind::Semi, "expected ';' at the end of expression");
  eatNextToken();
  return expr;
}

std::unique_ptr<hlx::DeclStmt> hlx::Parser::parseDeclStmt(){
  Token tok=nextToken;
  eatNextToken();

  matchOrReturn(TokenKind::Identifier, "expected identifier");
  varOrReturn(varDecl, parseVarDecl(tok.kind==TokenKind::KwLet));

  matchOrReturn(TokenKind::Semi, "expected ';' after declaration");
  eatNextToken();

  return std::make_unique<DeclStmt>(tok.location,std::move(varDecl));
}

std::unique_ptr<hlx::VarDecl> hlx::Parser::parseVarDecl(bool isLet){
  return nullptr;//TODO
}

std::unique_ptr<hlx::Expr> hlx::Parser::parsePrimary() {
  SourceLocation location = nextToken.location;

  if (nextToken.kind == TokenKind::Lpar) {
    eatNextToken(); // eat '('

    varOrReturn(expr, parseExpr());

    matchOrReturn(TokenKind::Rpar, "expected ')'");
    eatNextToken(); // eat ')'

    return std::make_unique<GroupingExpr>(location, std::move(expr));
  }

  if (nextToken.kind == TokenKind::Number) {
    auto literal = std::make_unique<NumberLiteral>(location, *nextToken.value);
    eatNextToken(); // eat NumberLiteral
    return literal;
  }

  if (nextToken.kind == TokenKind::Identifier) {
    auto declRefExpr =
        std::make_unique<DeclRefExpr>(location, *nextToken.value);
    eatNextToken(); // eat identifier

    if (nextToken.kind != TokenKind::Lpar)
      return declRefExpr;

    location = nextToken.location;

    varOrReturn(argumentList, parseArgumentList());

    return std::make_unique<CallExpr>(location, std::move(declRefExpr),
                                      std::move(*argumentList));
  }

  return report(location, "expected expression");
}

std::unique_ptr<std::vector<std::unique_ptr<hlx::Expr>>>
hlx::Parser::parseArgumentList() {
  matchOrReturn(TokenKind::Lpar, "expected '('");
  eatNextToken(); // eat (
  std::vector<std::unique_ptr<Expr>> argumentList;
  while (true) {
    if (nextToken.kind == TokenKind::Rpar)
      break;
    varOrReturn(expr, parseExpr());
    argumentList.emplace_back(std::move(expr));

    if (nextToken.kind != TokenKind::Comma)
      break;
    eatNextToken(); // eat ','
  }
  matchOrReturn(TokenKind::Rpar, "expected ')'");
  eatNextToken(); // eat ')'

  return std::make_unique<std::vector<std::unique_ptr<Expr>>>(
      std::move(argumentList));
}

std::unique_ptr<hlx::Expr> hlx::Parser::parseExpr() {
  varOrReturn(lhs, parsePrefixExpr());
  return parseExprRHS(std::move(lhs), 0);
}

std::unique_ptr<hlx::Expr> hlx::Parser::parseExprRHS(std::unique_ptr<Expr> lhs,
                                                     int precedence) {
  while (true) {
    Token op = nextToken;
    int curOpPrec = getTokPrecedence(op.kind);

    if (curOpPrec < precedence)
      return lhs;

    eatNextToken();
    varOrReturn(rhs, parsePrefixExpr());
    if (curOpPrec < getTokPrecedence(nextToken.kind)) {
      rhs = parseExprRHS(std::move(rhs), curOpPrec + 1);
      if (!rhs)
        return nullptr;
    }

    lhs = std::make_unique<BinaryOperator>(op.location, std::move(lhs),
                                           std::move(rhs), op.kind);
  }
}

std::unique_ptr<hlx::ParamDecl> hlx::Parser::parseParamDecl() {
  SourceLocation location = nextToken.location;
  std::string identifier = *nextToken.value;
  eatNextToken(); // eat ident

  matchOrReturn(TokenKind::Colon, "expected ':'");
  eatNextToken(); // eat :

  varOrReturn(type, parseType());

  return std::make_unique<hlx::ParamDecl>(location, std::move(identifier),
                                          std::move(*type));
}

std::unique_ptr<std::vector<std::unique_ptr<hlx::ParamDecl>>>
hlx::Parser::parseParameterList() {
  matchOrReturn(TokenKind::Lpar, "expected '('");
  eatNextToken(); // eat '('
  std::vector<std::unique_ptr<ParamDecl>> parameterList;

  while (true) {
    if (nextToken.kind == TokenKind::Rpar)
      break;

    matchOrReturn(TokenKind::Identifier, "expected parameter declaration");

    varOrReturn(paramDecl, parseParamDecl());
    parameterList.emplace_back(std::move(paramDecl));

    if (nextToken.kind != TokenKind::Comma)
      break;
    eatNextToken(); // eat ','
  }
  matchOrReturn(TokenKind::Rpar, "expected ')'");
  eatNextToken(); // eat ')'

  return std::make_unique<std::vector<std::unique_ptr<ParamDecl>>>(
      std::move(parameterList));
}

int hlx::Parser::getTokPrecedence(hlx::TokenKind tok) {
  switch (tok) {
  case hlx::TokenKind::Asterisk:
  case hlx::TokenKind::Slash:
  case hlx::TokenKind::Mod:
    return 6;
  case hlx::TokenKind::Plus:
  case hlx::TokenKind::Minus:
    return 5;
  case hlx::TokenKind::Gt:
  case hlx::TokenKind::Lt:
  case hlx::TokenKind::MoreThanEql:
  case hlx::TokenKind::LessThanEql:
    return 4;
  case hlx::TokenKind::EqualEqual:
  case hlx::TokenKind::NotEqual:
    return 3;
  case hlx::TokenKind::AmpAmp:
    return 2;
  case hlx::TokenKind::PipePipe:
    return 1;
  default:
    return -1;
  }
}

std::unique_ptr<hlx::Expr> hlx::Parser::parsePrefixExpr() {
  Token tok = nextToken;

  if (tok.kind != TokenKind::Excl && tok.kind != TokenKind::Minus)
    return parsePrimary();
  eatNextToken();

  varOrReturn(rhs, parsePrefixExpr());

  return std::make_unique<UnaryOperator>(tok.location, std::move(rhs),
                                         tok.kind);
}