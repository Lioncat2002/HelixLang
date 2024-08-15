#include <fstream>
#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <sstream>

#include "include/Lexer.h"
#include "include/Parser.h"
#include "include/Sema.h"
#include "include/Codegen.h"


int main(int argc, const char **argv) {
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
}