#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <llvm-14/llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <sstream>
#include <system_error>
#include <utility>
#include "src/core/lexer/Lexer.h"
#include "src/core/parser/Parser.h"
#include "src/core/sema/Sema.h"
#include "src/core/codegen/Codegen.h"
#include "src/utils/Driver.h"
#include "src/utils/Utils.h"

int main(int argc, const char **argv) {
    hlx::CompilerOptions options=hlx::parseArguments(argc, argv);

    if(options.displayHelp){
        hlx::displayHelp();
        return 0;
    }
    if(options.source.empty())
        hlx::error("no source file empty");

    if(options.source.extension()!=".hlx")
        hlx::error("unexpected source file extension");
    
    std::ifstream file(options.source);
    if(!file)
        hlx::error("failed to open '"+options.source.string()+'\'');

    std::stringstream buffer;
    buffer<<file.rdbuf();
    hlx::SourceFile sourceFile={options.source.c_str(),buffer.str()};

    hlx::Lexer lexer(sourceFile);
    hlx::Parser parser(lexer);

    auto [ast,success]=parser.parseSourceFile();

    if(options.astDump){
        for(auto &&fn:ast){
            fn->dump();
        }
        return 0;
    }

    if(!success)
        return 1;

    hlx::Sema sema(std::move(ast));
    auto resolvedTree=sema.resolveAST();

    if(options.resDump){
        for(auto &&fn:resolvedTree)
            fn->dump();
        return 0;
    }

    if (resolvedTree.empty()) {
        return 1;
    }

    hlx::Codegen codegen(std::move(resolvedTree),options.source.c_str());

    llvm::Module *llvmIR=codegen.generateIR();
    if(options.llvmDump){
        llvmIR->dump();
        return 0;
    }

    std::stringstream path;
    path<<"tmp-"<<std::filesystem::hash_value(options.source)<<".ll";
    const std::string &&llvmIRPath=path.str();

    std::error_code errorCode;
    llvm::raw_fd_ostream f(llvmIRPath,errorCode);
    llvmIR->print(f, nullptr);

    std::stringstream command;
    command<<"clang "<<llvmIRPath;
    if(!options.output.empty())
        command<<"-o"<<options.output;

    int ret=std::system(command.str().c_str());

    std::filesystem::remove(llvmIRPath);

    return ret;
    /*
    std::ifstream file(argv[1]);
    if(!file){
        std::cerr<<"Couldn't find file: "<<argv[1]<<'\n';
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    hlx::SourceFile sourceFile = {argv[1], buffer.str()};

    hlx::Lexer lexer(sourceFile);
    hlx::Parser parser(lexer);

    auto [ast, success] = parser.parseSourceFile();

    

    for (auto &&fn : ast)
    {

        fn->dump();
    }

    hlx::Sema sema(std::move(ast));

    auto res=sema.resolveAST();
    std::cerr<<"Resolved: \n";
    for (auto &&fn : res) {
        fn->dump();
    }
    hlx::Codegen codegen(std::move(res),argv[1]);
    
     std::error_code EC;
    
    // Open a file for writing
    llvm::raw_fd_ostream fileStream("intermediate.ll", EC, llvm::sys::fs::OpenFlags{});
    
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message() << "\n";
        return -1;
    }
    // Write the module to the file
    codegen.generateIR()->print(fileStream, nullptr);

    // Close the file
    fileStream.flush();

    //codegen.generateIR()->print(llvm::errs(), nullptr);

    return !success;
    */
}