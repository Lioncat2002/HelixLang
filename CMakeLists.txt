cmake_minimum_required(VERSION 3.22)
project(helix)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
include_directories(include "${LLVM_INCLUDE_DIR}")

llvm_map_components_to_libnames(llvm_libs core)



add_executable(helixlang main.cpp
        src/core/lexer/Lexer.h
        src/utils/Utils.h
        src/core/lexer/Lexer.cpp
        src/core/ast/Ast.h
        src/core/ast/Ast.cpp
        src/core/ast/ResolvedAst.h
        src/core/ast/ResolvedAst.cpp
        src/core/parser/Parser.h
        src/core/parser/Parser.cpp
        src/utils/Utils.cpp
        src/core/sema/Sema.h
        src/core/sema/Sema.cpp
        src/core/codegen/Codegen.h
        src/core/codegen/Codegen.cpp
        src/utils/Driver.h
        src/utils/Driver.cpp
        )
target_link_libraries(helixlang LLVM-14)