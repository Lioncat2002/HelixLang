#include "Ast.h"
#include <cstddef>
#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <string_view>

void hlx::FunctionDecl::dump(size_t level) const {
  std::cerr << indent(level) << "FunctionDecl: " << identifier << " : "
            << type.name << '\n';

  for (auto &&param : params)
    param->dump(level + 1);

  body->dump(level + 1);
}

void hlx::VarDecl::dump(size_t level)const{
   std::cerr << indent(level) << "VarDecl: " << identifier;
  if (type)
    std::cerr << ':' << type->name;
  std::cerr << '\n';

  if (initializer)
    initializer->dump(level + 1);
}

void hlx::DeclStmt::dump(size_t level) const {
  std::cerr << indent(level) << "DeclStmt:\n";
  varDecl->dump(level + 1);
}

void hlx::Block::dump(size_t level) const {
  std::cerr << indent(level) << "Block\n";

  for (auto &&stmt : statements)
    stmt->dump(level + 1);
}

void hlx::ReturnStmt::dump(size_t level) const {
  std::cerr << indent(level) << "ReturnStmt\n";
  if (expr)
    expr->dump(level + 1);
}

void hlx::NumberLiteral::dump(size_t level) const {
  std::cerr << indent(level) << "NumberLiteral: '" << value << "'\n";
}

void hlx::DeclRefExpr::dump(size_t level) const {
  std::cerr << indent(level) << "DeclRefExpr: '" << identifier << "'\n";
}

void hlx::CallExpr::dump(size_t level) const {
  std::cerr << indent(level) << "CallExpr:\n";
  identifier->dump(level + 1);
  for (auto &&arg : arguments) {
    arg->dump(level + 1);
  }
}

void hlx::ParamDecl::dump(size_t level) const {
  std::cerr << indent(level) << "ParamDecl: " << identifier << ':' << type.name
            << '\n';
}

std::string_view hlx::getOpStr(hlx::TokenKind op) {
  if (op == TokenKind::Plus)
    return "+";
  if (op == TokenKind::Minus)
    return "-";
  if (op == TokenKind::Asterisk)
    return "*";
  if (op == TokenKind::Slash)
    return "/";
  if (op == TokenKind::EqualEqual)
    return "==";
  if(op==TokenKind::NotEqual)
    return "!=";
  if (op == TokenKind::AmpAmp)
    return "&&";
  if (op == TokenKind::PipePipe)
    return "||";
  if (op == TokenKind::Lt)
    return "<";
  if (op == TokenKind::Gt)
    return ">";
  if (op == TokenKind::Excl)
    return "!";

  llvm_unreachable("unexpected operator");
}

void hlx::BinaryOperator::dump(size_t level) const {
  std::cerr << indent(level) << "BinaryOperator: '" << getOpStr(op) << '\''
            << '\n';
  lhs->dump(level + 1);
  rhs->dump(level + 1);
}

void hlx::UnaryOperator::dump(size_t level) const {
  std::cerr << indent(level) << "BinaryOperator: '" << getOpStr(op) << '\''
            << '\n';
  operand->dump(level + 1);
}

void hlx::GroupingExpr::dump(size_t level) const {
  std::cerr << indent(level) << "GroupingExpr:\n";

  expr->dump(level + 1);
}

void hlx::IfStmt::dump(size_t level) const{
  std::cerr<<indent(level)<<"IfStmt\n";
  condition->dump(level+1);
  trueBlock->dump(level+1);
  if(falseBlock)
    falseBlock->dump(level+1);
}

void hlx::WhileStmt::dump(size_t level)const{
  std::cerr<<indent(level)<<"WhileStmt\n";

  condition->dump(level+1);
  body->dump(level+1);
}