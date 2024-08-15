cmake_minimum_required(VERSION 3.22)
project(helixlang)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
include_directories(include "${LLVM_INCLUDE_DIR}")

llvm_map_components_to_libnames(llvm_libs core)



add_executable(helixlang main.cpp
        include/Lexer.h
        include/Utils.h
        src/Lexer.cpp
        include/Ast.h
        src/Ast.cpp
        include/Parser.h
        src/Parser.cpp
        src/Utils.cpp
        include/Sema.h
        src/Sema.cpp
        include/Codegen.h
        src/Codegen.cpp
        )
target_link_libraries(helixlang LLVM-14)