#pragma once
#include "Ast.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <map>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

namespace hlx {
    class Codegen {
      llvm::LLVMContext context;
      llvm::IRBuilder<> builder;
      llvm::Module _module;

      std::vector<std::unique_ptr<hlx::ResolvedFunctionDecl>> resolvedTree;
      std::map<const ResolvedDecl *, llvm::Value *> declarations;

      public:
      Codegen(std::vector<std::unique_ptr<ResolvedFunctionDecl>> resolvedTree,std::string_view sourcePath)
      : resolvedTree(std::move(resolvedTree)),
      builder(context),
      _module("<translation_unit>", context){
        _module.setSourceFileName(sourcePath);
        _module.setTargetTriple("x86_64-pc-linux-gnu");
      }

      llvm::Module *generateIR();
      llvm::Type *generateType(Type type);
      llvm::Instruction *allocaInsertPoint;
      llvm::Value *retVal=nullptr;
      llvm::BasicBlock *retBB=nullptr;
      void generateFunctionDecl(const ResolvedFunctionDecl &functionDecl);
      void generateFunctionBody(const ResolvedFunctionDecl &functionDecl);
      llvm::AllocaInst *allocateStackVariable(llvm::Function *function,const std::string_view identifier);
      void generateBlock(const ResolvedBlock &block);
      llvm::Value *generateStmt(const ResolvedStmt &stmt);
      llvm::Value *generateReturnStmt(const ResolvedReturnStmt &stmt);
      llvm::Value *generateExpr(const ResolvedExpr &expr);
      llvm::Value *generateCallExpr(const ResolvedCallExpr &call);
      void generateBuiltinPrintBody(const ResolvedFunctionDecl &println);
      void generateMainWrapper();
    };
} // namespace hlx