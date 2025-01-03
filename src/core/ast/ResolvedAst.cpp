#include "ResolvedAst.h"
#include "Ast.h"
#include <cstddef>
#include <iostream>
void hlx::ResolvedNumberLiteral::dump(size_t level) const {
    std::cerr << indent(level) << "ResolvedNumberLiteral: '" << value << "'\n";
}

void hlx::ResolvedDeclRefExpr::dump(size_t level) const {
    std::cerr << indent(level) << "ResolvedDeclRefExpr: @(" << decl << ") "
              << decl->identifier << '\n';
}

void hlx::ResolvedCallExpr::dump(size_t level) const {
    std::cerr << indent(level) << "ResolvedCallExpr: @(" << callee << ") "
              << callee->identifier << '\n';

    for (auto &&arg : arguments)
        arg->dump(level + 1);
}

void hlx::ResolvedBlock::dump(size_t level) const {
    std::cerr << indent(level) << "ResolvedBlock\n";

    for (auto &&stmt : statements)
        stmt->dump(level + 1);
}

void hlx::ResolvedParamDecl::dump(size_t level) const {
    std::cerr << indent(level) << "ResolvedParamDecl: @(" << this << ") "
              << identifier << ':' << '\n';
}

void hlx::ResolvedFunctionDecl::dump(size_t level) const {
    std::cerr << indent(level) << "ResolvedFunctionDecl: @(" << this << ") "
              << identifier << ':' << '\n';

    for (auto &&param : params)
        param->dump(level + 1);

    body->dump(level + 1);
}

void hlx::ResolvedReturnStmt::dump(size_t level) const {
    std::cerr << indent(level) << "ResolvedReturnStmt\n";

    if (expr)
        expr->dump(level + 1);
}

void hlx::ResolvedBinaryOperator::dump(size_t level)const{
    std::cerr<<indent(level)<<"ResolvedBinaryOperator: '"<<getOpStr(op)
    <<'\''<<'\n';

    lhs->dump(level+1);
    rhs->dump(level+1);
}

void hlx::ResolvedUnaryOperator::dump(size_t level)const{
    std::cerr << indent(level) << "ResolvedUnaryOperator: '" << getOpStr(op)
            << '\'' << '\n';

  operand->dump(level + 1);
}

void hlx::ResolvedGroupingExpr::dump(size_t level)const{
    std::cerr<<indent(level)<<"ResolvedGroupingExpr:\n";

    expr->dump(level+1);
}

void hlx::ResolvedIfStmt::dump(size_t level) const{
    std::cerr<<indent(level)<<"ResolvedIfStmt:\n";

    condition->dump(level+1);
    trueBlock->dump(level+1);
    if(falseBlock)
        falseBlock->dump(level+1);
}

void hlx::ResolvedWhileStmt::dump(size_t level) const{
    std::cerr<<indent(level)<<"ResolvedWhileStmt\n";

    condition->dump(level+1);
    body->dump(level+1);
}

void hlx::ResolvedVarDecl::dump(size_t level) const {
  std::cerr <<indent(level) << "ResolvedVarDecl: @(" << this << ") "
            << identifier << ':' << '\n';
  if (initializer)
    initializer->dump(level + 1);
}

void hlx::ResolvedDeclStmt::dump(size_t level) const{
    std::cerr<<indent(level)<<"ResolvedDeclStmt:\n";
    varDecl->dump(level+1);
}

void hlx::ResolvedAssignment::dump(size_t level) const {
  std::cerr << indent(level) << "ResolvedAssignment:\n";
  variable->dump(level + 1);
  expr->dump(level + 1);
}