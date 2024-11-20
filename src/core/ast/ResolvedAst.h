#pragma once
#include "Ast.h"
#include <cstddef>
#include <memory>
#include <utility>
namespace hlx{
    
struct ResolvedStmt:public Dumpable {
  SourceLocation location;

  ResolvedStmt(SourceLocation location) : location(location) {}

  virtual ~ResolvedStmt() = default;
  virtual void dump(size_t level = 0) const = 0;
};

struct ResolvedExpr : public ResolvedStmt {
  Type type;

  ResolvedExpr(SourceLocation location, Type type)
      : ResolvedStmt(location), type(type) {}
};

struct ResolvedDecl {
  SourceLocation location;
  std::string identifier;
  Type type;

  ResolvedDecl(SourceLocation location, std::string identifier, Type type)
      : location(location), identifier(std::move(identifier)), type(type) {}

  virtual ~ResolvedDecl() = default;

  virtual void dump(size_t level = 0) const = 0;
};



struct ResolvedNumberLiteral : public ResolvedExpr {
  double value;
  ResolvedNumberLiteral(SourceLocation location, double value)
      : ResolvedExpr(location, Type::builtinNumber()), value(value) {}

  void dump(size_t level = 0) const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ResolvedDeclRefExpr : public ResolvedExpr {
  const ResolvedDecl *decl;
  ResolvedDeclRefExpr(SourceLocation location, ResolvedDecl &decl)
      : ResolvedExpr(location, decl.type), decl(&decl) {}

  void dump(size_t level = 0) const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ResolvedBlock {
  SourceLocation location;
  std::vector<std::unique_ptr<ResolvedStmt>> statements;

  ResolvedBlock(SourceLocation location,
                std::vector<std::unique_ptr<ResolvedStmt>> statements)
      : location(location), statements(std::move(statements)) {}
  void dump(size_t level = 0) const;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ResolvedParamDecl : public ResolvedDecl {
  ResolvedParamDecl(SourceLocation location, std::string identifier, Type type)
      : ResolvedDecl(location, identifier, type) {}
  void dump(size_t level = 0) const;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ResolvedFunctionDecl : public ResolvedDecl {
  std::vector<std::unique_ptr<ResolvedParamDecl>> params;
  std::unique_ptr<ResolvedBlock> body;

  ResolvedFunctionDecl(SourceLocation location, std::string identifier,
                       Type type,
                       std::vector<std::unique_ptr<ResolvedParamDecl>> params,
                       std::unique_ptr<ResolvedBlock> body)
      : ResolvedDecl(location, std::move(identifier), type),
        params(std::move(params)), body(std::move(body)) {}
  void dump(size_t level = 0) const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ResolvedCallExpr : public ResolvedExpr {
  const ResolvedFunctionDecl *callee;
  std::vector<std::unique_ptr<ResolvedExpr>> arguments;

  ResolvedCallExpr(SourceLocation location, const ResolvedFunctionDecl &callee,
                   std::vector<std::unique_ptr<ResolvedExpr>> arguments)
      : ResolvedExpr(location, callee.type), callee(&callee),
        arguments(std::move(arguments)) {}
  void dump(size_t level = 0) const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ResolvedReturnStmt : public ResolvedStmt {
  std::unique_ptr<ResolvedExpr> expr;

  ResolvedReturnStmt(SourceLocation location,
                     std::unique_ptr<ResolvedExpr> expr = nullptr)
      : ResolvedStmt(location), expr(std::move(expr)) {}
  void dump(size_t level = 0) const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ResolvedBinaryOperator:public ResolvedExpr{
  TokenKind op;
  std::unique_ptr<ResolvedExpr> lhs;
  std::unique_ptr<ResolvedExpr> rhs;

  ResolvedBinaryOperator(SourceLocation location,
                          TokenKind op,
                          std::unique_ptr<ResolvedExpr> lhs,
                          std::unique_ptr<ResolvedExpr> rhs)
                          : ResolvedExpr(location, lhs->type),
                          op(op),
                          lhs(std::move(lhs)),
                          rhs(std::move(rhs)){}
  
  void dump(size_t level=0)const override;
};

struct ResolvedUnaryOperator:public ResolvedExpr{
  TokenKind op;
  std::unique_ptr<ResolvedExpr> operand;

  ResolvedUnaryOperator(SourceLocation location,
                        TokenKind op,
                        std::unique_ptr<ResolvedExpr> operand)
                        : ResolvedExpr(location,operand->type),
                        op(op),
                        operand(std::move(operand)){}
  
  void dump(size_t level=0)const override;
};

struct ResolvedGroupingExpr:public ResolvedExpr{
  std::unique_ptr<ResolvedExpr> expr;

  ResolvedGroupingExpr(SourceLocation location,
                        std::unique_ptr<ResolvedExpr> expr)
                        :ResolvedExpr(location, expr->type),
                        expr(std::move(expr)){}
  
  void dump(size_t level=0)const override;
};

struct ResolvedIfStmt:public ResolvedStmt{
  std::unique_ptr<ResolvedExpr> condition;
  std::unique_ptr<ResolvedBlock> trueBlock;
  std::unique_ptr<ResolvedBlock> falseBlock;

  ResolvedIfStmt(SourceLocation location,
                  std::unique_ptr<ResolvedExpr> condition,
                  std::unique_ptr<ResolvedBlock> trueBlock,
                  std::unique_ptr<ResolvedBlock> falseBlock=nullptr)
                  : ResolvedStmt(location),
                  condition(std::move(condition)),
                  trueBlock(std::move(trueBlock)),
                  falseBlock(std::move(falseBlock)){}
  void dump(size_t level = 0) const override;
};

struct ResolvedWhileStmt:public ResolvedStmt{
  std::unique_ptr<ResolvedExpr> condition;
  std::unique_ptr<ResolvedBlock> body;

  ResolvedWhileStmt(SourceLocation location,
                    std::unique_ptr<ResolvedExpr> condition,
                    std::unique_ptr<ResolvedBlock> body)
                    :ResolvedStmt(location),
                    condition(std::move(condition)),
                    body(std::move(body)){}
  
  void dump(size_t level=0)const override;
};

struct ResolvedVarDecl:public ResolvedDecl{
  std::unique_ptr<ResolvedExpr> initializer;
  bool isMutable;

  ResolvedVarDecl(SourceLocation location,
                  std::string identifier,
                  Type type,
                  bool isMutable,
                  std::unique_ptr<ResolvedExpr> initializer=nullptr)
                  : ResolvedDecl(location,std::move(identifier),type),
                  initializer(std::move(initializer)),
                  isMutable(isMutable){}
  
  void dump(size_t level=0)const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ResolvedDeclStmt:public ResolvedStmt{
  std::unique_ptr<ResolvedVarDecl> varDecl;
  
  ResolvedDeclStmt(SourceLocation location,
                    std::unique_ptr<ResolvedVarDecl> varDecl)
                    : ResolvedStmt(location),
                      varDecl(std::move(varDecl)){}
  
  void dump(size_t level=0)const override;
};

}