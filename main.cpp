#include <fstream>
#include <iostream>
#include <sstream>

#include "include/Lexer.h"
#include "include/Parser.h"
#include "include/Sema.h"


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


    return !success;
}