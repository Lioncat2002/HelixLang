#include "ResolvedAst.h"
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