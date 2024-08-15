#pragma once
#include "Ast.h"
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

}