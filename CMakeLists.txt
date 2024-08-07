cmake_minimum_required(VERSION 3.26)
project(helixlang)

set(CMAKE_CXX_STANDARD 17)

add_executable(helixlang main.cpp
        include/Lexer.h
        include/Utils.h
        src/Lexer.cpp
        src/Lexer.cpp
        include/Ast.h
        include/Ast.h
        src/Ast.cpp
        src/Ast.cpp
        include/Parser.h
        include/Parser.h
        src/Parser.cpp
        src/Parser.cpp
        src/Utils.cpp
        src/Utils.cpp
        include/Sema.h
        include/Sema.h
        src/Sema.cpp
        src/Sema.cpp)
