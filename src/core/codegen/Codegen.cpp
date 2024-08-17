#include "Codegen.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/ErrorHandling.h>
#include <memory>
#include <string_view>
#include <vector>

llvm::Module *hlx::Codegen::generateIR() {
  for (auto &&function : resolvedTree) {
    generateFunctionDecl(*function);
  }

  for (auto &&function : resolvedTree) {
    generateFunctionBody(*function);
  }

  generateMainWrapper();

  return &_module;
}

void hlx::Codegen::generateMainWrapper() {
  auto *builtinMain = _module.getFunction("main");
  builtinMain->setName("__builtin_main");

  auto *main = llvm::Function::Create(
      llvm::FunctionType::get(builder.getInt32Ty(), {}, false),
      llvm::Function::ExternalLinkage, "main", _module);

  auto *entry = llvm::BasicBlock::Create(context, "entry", main);
  builder.SetInsertPoint(entry);

  builder.CreateCall(builtinMain);
  builder.CreateRet(llvm::ConstantInt::getSigned(builder.getInt32Ty(), 0));
}

void hlx::Codegen::generateFunctionDecl(
    const ResolvedFunctionDecl &functionDecl) {
  auto *retType = generateType(functionDecl.type);

  std::vector<llvm::Type *> paramTypes;
  for (auto &&param : functionDecl.params) {
    paramTypes.emplace_back(generateType(param->type));
  }

  auto *type = llvm::FunctionType::get(retType, paramTypes, false);

  llvm::Function::Create(type, llvm::Function::ExternalLinkage,
                         functionDecl.identifier, _module);
}

void hlx::Codegen::generateFunctionBody(
    const ResolvedFunctionDecl &functionDecl) {
  auto *function = _module.getFunction(functionDecl.identifier);

  auto *entryBB = llvm::BasicBlock::Create(context, "entry", function);
  builder.SetInsertPoint(entryBB);

  // Note: llvm:Instruction has a protected destructor.
  llvm::Value *undef = llvm::UndefValue::get(builder.getInt32Ty());
  allocaInsertPoint = new llvm::BitCastInst(undef, undef->getType(),
                                            "alloca.placeholder", entryBB);

  bool isVoid = functionDecl.type.kind == Type::Kind::Void;
  if (!isVoid)
    retVal = allocateStackVariable(function, "retval");
  retBB = llvm::BasicBlock::Create(context, "return");

  int idx = 0;
  for (auto &&arg : function->args()) {
    const auto *paramDecl = functionDecl.params[idx].get();
    arg.setName(paramDecl->identifier);

    llvm::Value *var = allocateStackVariable(function, paramDecl->identifier);
    builder.CreateStore(&arg, var);

    declarations[paramDecl] = var;
    ++idx;
  }

  if (functionDecl.identifier == "println")
    generateBuiltinPrintBody(functionDecl);
  else
    generateBlock(*functionDecl.body);

  if (retBB->hasNPredecessorsOrMore(1)) {
    builder.CreateBr(retBB);
    retBB->insertInto(function);
    builder.SetInsertPoint(retBB);
  }

  allocaInsertPoint->eraseFromParent();
  allocaInsertPoint = nullptr;

  if (isVoid) {
    builder.CreateRetVoid();
    return;
  }

  builder.CreateRet(builder.CreateLoad(builder.getDoubleTy(), retVal));
}

llvm::Type *hlx::Codegen::generateType(hlx::Type type) {
  if (type.kind == Type::Kind::Number)
    return builder.getDoubleTy();
  return builder.getVoidTy();
}
llvm::AllocaInst *
hlx::Codegen::allocateStackVariable(llvm::Function *function,
                                    const std::string_view identifier) {
  llvm::IRBuilder<> tmpBuilder(context);
  tmpBuilder.SetInsertPoint(allocaInsertPoint);
  return tmpBuilder.CreateAlloca(tmpBuilder.getDoubleTy(), nullptr, identifier);
}

void hlx::Codegen::generateBlock(const hlx::ResolvedBlock &block) {
  for (auto &&stmt : block.statements) {
    generateStmt(*stmt);
    if (dynamic_cast<const ResolvedReturnStmt *>(stmt.get())) {
      builder.ClearInsertionPoint();
      break;
    }
  }
}

llvm::Value *hlx::Codegen::generateStmt(const hlx::ResolvedStmt &stmt) {
  if (auto *expr = dynamic_cast<const ResolvedExpr *>(&stmt)) {
    return generateExpr(*expr);
  }

  if (auto *returnStmt = dynamic_cast<const ResolvedReturnStmt *>(&stmt)) {
    return generateReturnStmt(*returnStmt);
  }

  llvm_unreachable("unknown statement");
}

llvm::Value *hlx::Codegen::generateReturnStmt(const ResolvedReturnStmt &stmt) {
  if (stmt.expr)
    builder.CreateStore(generateExpr(*stmt.expr), retVal);

  return builder.CreateBr(retBB);
}

llvm::Value *hlx::Codegen::generateExpr(const ResolvedExpr &expr) {

  if (auto *number = dynamic_cast<const ResolvedNumberLiteral *>(&expr)) {
    return llvm::ConstantFP::get(builder.getDoubleTy(), number->value);
  }

  if (auto *dre = dynamic_cast<const ResolvedDeclRefExpr *>(&expr)) {
    return builder.CreateLoad(builder.getDoubleTy(), declarations[dre->decl]);
  }

  if (auto *call = dynamic_cast<const ResolvedCallExpr *>(&expr))
    return generateCallExpr(*call);

  if (auto *binop = dynamic_cast<const ResolvedBinaryOperator *>(&expr))
    return generateBinaryOperator(*binop);

  if (auto *unop = dynamic_cast<const ResolvedUnaryOperator *>(&expr))
    return generateUnaryOperator(*unop);

  if (auto *grouping = dynamic_cast<const ResolvedGroupingExpr *>(&expr))
    return generateExpr(*grouping->expr);

  llvm_unreachable("unexpected expression");
}

llvm::Value *
hlx::Codegen::generateUnaryOperator(const ResolvedUnaryOperator &unop) {
  llvm::Value *operand = generateExpr(*unop.operand);

  if (unop.op == TokenKind::Minus)
    return builder.CreateFNeg(operand);

  if (unop.op == TokenKind::Excl)
    return boolToDouble(builder.CreateNot(doubleToBool(operand)));

  llvm_unreachable("unknown unary op");
  return nullptr;
}

llvm::Value *
hlx::Codegen::generateBinaryOperator(const ResolvedBinaryOperator &binop) {
  TokenKind op = binop.op;

  llvm::Value *lhs = generateExpr(*binop.lhs);
  llvm::Value *rhs = generateExpr(*binop.rhs);

  if (op == TokenKind::Plus)
    return builder.CreateFAdd(lhs, rhs);
  if (op == TokenKind::Minus)
    return builder.CreateFSub(lhs, rhs);
  if (op == TokenKind::Asterisk)
    return builder.CreateFMul(lhs, rhs);
  if (op == TokenKind::Slash)
    return builder.CreateFDiv(lhs, rhs);
  if (op == TokenKind::Lt)
    return boolToDouble(builder.CreateFCmpOLT(lhs, rhs));
  if (op == TokenKind::Gt)
    return boolToDouble(builder.CreateFCmpOGT(lhs, rhs));
  if (op == TokenKind::EqualEqual)
    return boolToDouble(builder.CreateFCmpOEQ(lhs, rhs));
  if (op == TokenKind::AmpAmp || op == TokenKind::PipePipe) {
  }

  llvm_unreachable("unexpected binary operator");
  return nullptr;
}

llvm::Value *hlx::Codegen::generateCallExpr(const ResolvedCallExpr &call) {
  llvm::Function *callee = _module.getFunction(call.callee->identifier);

  std::vector<llvm::Value *> args;
  for (auto &&arg : call.arguments) {
    args.emplace_back(generateExpr(*arg));
  }

  return builder.CreateCall(callee, args);
}

void hlx::Codegen::generateBuiltinPrintBody(
    const ResolvedFunctionDecl &println) {
  auto *type = llvm::FunctionType::get(builder.getInt32Ty(),
                                       {builder.getInt8PtrTy()}, true);

  auto *printf = llvm::Function::Create(type, llvm::Function::ExternalLinkage,
                                        "printf", _module);

  auto *format = builder.CreateGlobalStringPtr("%.15g\n");

  llvm::Value *param = builder.CreateLoad(
      builder.getDoubleTy(), declarations[println.params[0].get()]);

  builder.CreateCall(printf, {format, param});
}

llvm::Value *hlx::Codegen::doubleToBool(llvm::Value *v) {
  return builder.CreateFCmpONE(
      v, llvm::ConstantFP::get(builder.getDoubleTy(), 0.0), "to.bool");
}

llvm::Value *hlx::Codegen::boolToDouble(llvm::Value *v) {
  return builder.CreateUIToFP(v, builder.getDoubleTy(), "to.double");
}