#pragma once

#include <optional>
#include "Ast.h"

namespace hlx{

    class Sema{
        std::vector<std::unique_ptr<FunctionDecl>> ast;
        std::vector<std::vector<ResolvedDecl*>> scopes;

        ResolvedFunctionDecl *currentFunction;


    public:
        explicit Sema(std::vector<std::unique_ptr<FunctionDecl>> ast)
        :ast(std::move(ast)){}
        std::unique_ptr<ResolvedFunctionDecl> resolveFunctionDeclaration(const FunctionDecl &function);
        std::unique_ptr<ResolvedCallExpr> resolveCallExpr(const CallExpr &call);
        std::unique_ptr<ResolvedDeclRefExpr> resolveDeclRefExpr(const DeclRefExpr &declRefExpr,bool isCallee=false);
        std::unique_ptr<ResolvedExpr> resolveExpr(const Expr &expr);
        std::unique_ptr<ResolvedBlock> resolveBlock(const Block &block);
        std::unique_ptr<ResolvedParamDecl> resolveParamDecl(const ParamDecl &param);
        std::unique_ptr<ResolvedStmt> resolveStmt(const Stmt &stmt);
        std::unique_ptr<ResolvedReturnStmt> resolveReturnStmt(const ReturnStmt &returnStmt);
        std::optional<Type> resolveType(Type parsedType);
        std::vector<std::unique_ptr<ResolvedFunctionDecl>> resolveSourceFile();
        std::vector<std::unique_ptr<ResolvedFunctionDecl>> resolveAST();
        std::unique_ptr<ResolvedFunctionDecl> createBuiltinPrintln();
        std::pair<ResolvedDecl *,int> lookupDecl(const std::string id);
        bool insertDeclToCurrentScope(ResolvedDecl &decl);

        class ScopeRAII{
            Sema *sema;

        public:
            explicit ScopeRAII(Sema *sema)
            : sema(sema){
                sema->scopes.emplace_back();
            }
            ~ScopeRAII(){sema->scopes.pop_back();}
        };
    };



}
