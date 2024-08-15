#pragma once

#include "../../utils/Utils.h"
#include <memory>
#include <vector>

namespace hlx {
struct Decl:public Dumpable {
  SourceLocation location;
  std::string identifier;

  Decl(SourceLocation location, std::string identifier)
      : location(location), identifier(std::move(identifier)) {}

  virtual ~Decl() = default;
};
struct Stmt:public Dumpable {
  SourceLocation location;
  Stmt(SourceLocation location) : location(location) {}
  virtual ~Stmt() = default;
};

struct Expr : public Stmt {
  Expr(SourceLocation location) : Stmt(location) {}
};

struct DeclRefExpr : public Expr {
  std::string identifier;
  DeclRefExpr(SourceLocation location, std::string identifer)
      : Expr(location), identifier(identifer) {}
  void dump(size_t level = 0) const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct CallExpr : public Expr {
  std::unique_ptr<DeclRefExpr> identifier;
  std::vector<std::unique_ptr<Expr>> arguments;

  CallExpr(SourceLocation location, std::unique_ptr<DeclRefExpr> identifier,
           std::vector<std::unique_ptr<Expr>> arguments)
      : Expr(location), identifier(std::move(identifier)),
        arguments(std::move(arguments)) {}
  void dump(size_t level = 0) const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct NumberLiteral : public Expr {
  std::string value;
  NumberLiteral(SourceLocation location, std::string value)
      : Expr(location), value(value) {}

  void dump(size_t level = 0) const override;
  std::string indent(size_t level) const { return std::string(level * 2, ' '); }
};

struct ReturnStmt : public Stmt {
  std::unique_ptr<Expr> expr;
  ReturnStmt(SourceLocation location, std::unique_ptr<Expr> expr = nullptr)
      : Stmt(location), expr(std::move(expr)) {}

  void dump(size_t level = 0) const override;
};

struct Block:public Dumpable {
  SourceLocation location;
  std::vector<std::unique_ptr<Stmt>> statements;
  Block(SourceLocation location, std::vector<std::unique_ptr<Stmt>> statements)
      : location(location), statements(std::move(statements)) {}
  void dump(size_t level = 0) const;
};
struct Type {
  enum class Kind { Void, KwNumber, Number, Custom };
  Kind kind;
  std::string name;

  static Type builtinVoid() { return {Kind::Void, "void"}; }
  static Type builtinKwNumber() {return {Kind::KwNumber,"number"};}
  static Type builtinNumber() { return {Kind::Number, "number"}; }
  static Type custom(const std::string &name) { return {Kind::Custom, name}; }

private:
  Type(Kind kind, std::string name) : kind(kind), name(std::move(name)){};
};

struct ParamDecl : public Decl {
  Type type;
  ParamDecl(SourceLocation location, std::string identifier, Type type)
      : Decl(location, std::move(identifier)), type(std::move(type)) {}
  void dump(size_t level = 0) const override;
};

struct FunctionDecl : public Decl {
  Type type;
  std::unique_ptr<Block> body;
  std::vector<std::unique_ptr<ParamDecl>> params;

  FunctionDecl(SourceLocation location, std::string identifier, Type type,
               std::unique_ptr<Block> body,
               std::vector<std::unique_ptr<ParamDecl>> params)
      : Decl(location, std::move(identifier)), type(std::move(type)),
        body(std::move(body)), params(std::move(params)) {}

  void dump(size_t level = 0) const override;
};

} // namespace hlx
