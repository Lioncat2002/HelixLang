#pragma once

#include <memory>
#include <vector>
#include "../../utils/Utils.h"
namespace hlx{
    struct Stmt{
        SourceLocation location;
        Stmt(SourceLocation location)
                : location(location){}
        virtual ~Stmt()=default;
        virtual void dump(size_t level=0)const=0;
    };

    struct Expr: public Stmt{
        Expr(SourceLocation location)
                : Stmt(location){}
    };

    struct DeclRefExpr:public Expr{
        std::string identifier;
        DeclRefExpr(SourceLocation location,std::string identifer)
        : Expr(location),
        identifier(identifer){}
        void dump(size_t level=0) const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct CallExpr:public Expr{
        std::unique_ptr<DeclRefExpr> identifier;
        std::vector<std::unique_ptr<Expr>> arguments;

        CallExpr(SourceLocation location,
                 std::unique_ptr<DeclRefExpr> identifier,
                 std::vector<std::unique_ptr<Expr>> arguments)
                : Expr(location),
                  identifier(std::move(identifier)),
                  arguments(std::move(arguments)){}
        void dump(size_t level=0) const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct NumberLiteral:public Expr{
        std::string value;
        NumberLiteral(SourceLocation location,std::string value)
        : Expr(location),
        value(value){}

        void dump(size_t level=0) const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct ReturnStmt:public Stmt{
        std::unique_ptr<Expr> expr;
        ReturnStmt(SourceLocation location,std::unique_ptr<Expr> expr=nullptr)
        : Stmt(location),
        expr(std::move(expr)){}

        void dump(size_t level=0) const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct Block{
        SourceLocation location;
        std::vector<std::unique_ptr<Stmt>> statements;
        Block(SourceLocation location,
              std::vector<std::unique_ptr<Stmt>> statements)
              : location(location),
                statements(std::move(statements)){}
        void dump(size_t level=0) const;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };
    struct Type{
        enum class Kind {Void,KwNumber,Number,Custom};
        Kind kind;
        std::string name;

        static Type builtinVoid(){return {Kind::Void,"void"};}
        static Type builtinNumber(){return {Kind::Number,"number"};}
        static Type custom(const std::string &name){return {Kind::Custom,name};}

    private:
        Type(Kind kind, std::string name) : kind(kind), name(std::move(name)){};

    };
    struct Decl{
        SourceLocation location;
        std::string identifier;

        Decl(SourceLocation location,std::string identifier)
        :location(location),
        identifier(std::move(identifier)){}

        virtual ~Decl()=default;

        virtual void dump(size_t level=0) const =0;
    };

    struct ParamDecl:public Decl{
        Type type;
        ParamDecl(SourceLocation location,std::string identifier,Type type)
        : Decl(location,std::move(identifier)),
          type(std::move(type)){}
        void dump(size_t level=0)const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct FunctionDecl:public Decl{
        Type type;
        std::unique_ptr<Block> body;
        std::vector<std::unique_ptr<ParamDecl>> params;

        FunctionDecl(SourceLocation location,
                     std::string identifier,
                     Type type,
                     std::unique_ptr<Block> body,
                     std::vector<std::unique_ptr<ParamDecl>> params)
        : Decl(location,std::move(identifier)),
        type(std::move(type)),
        body(std::move(body)),
        params(std::move(params)){}

        void dump(size_t level=0) const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct ResolvedStmt{
        SourceLocation location;

        ResolvedStmt(SourceLocation location): location(location){}

        virtual ~ResolvedStmt()=default;
        virtual void dump(size_t level=0)const =0;
    };

    struct ResolvedExpr: public ResolvedStmt{
        Type type;

        ResolvedExpr(SourceLocation location,Type type)
                : ResolvedStmt(location), type(type){}
    };

    struct ResolvedDecl{
        SourceLocation location;
        std::string identifier;
        Type type;

        ResolvedDecl(SourceLocation location,std::string identifier,Type type)
        : location(location),
          identifier(std::move(identifier)),
          type(type){}

        virtual ~ResolvedDecl()=default;

        virtual void dump(size_t level=0) const=0;
    };

    struct ResolvedNumberLiteral:public ResolvedExpr{
        double value;
        ResolvedNumberLiteral(SourceLocation location,double value)
        : ResolvedExpr(location,Type::builtinNumber()),
          value(value){}

        void dump(size_t level=0)const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct ResolvedDeclRefExpr:public ResolvedExpr{
        const ResolvedDecl *decl;
        ResolvedDeclRefExpr(SourceLocation location,ResolvedDecl &decl)
        : ResolvedExpr(location,decl.type),
        decl(&decl){}

        void dump(size_t level=0)const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct ResolvedBlock{
        SourceLocation location;
        std::vector<std::unique_ptr<ResolvedStmt>> statements;

        ResolvedBlock(SourceLocation location,
                      std::vector<std::unique_ptr<ResolvedStmt>> statements)
                : location(location),
                  statements(std::move(statements)){}
        void dump(size_t level=0)const;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct ResolvedParamDecl:public ResolvedDecl{
        ResolvedParamDecl(SourceLocation location,std::string identifier,Type type)
                : ResolvedDecl(location,identifier,type){}
        void dump(size_t level=0)const;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct ResolvedFunctionDecl:public ResolvedDecl{
        std::vector<std::unique_ptr<ResolvedParamDecl>> params;
        std::unique_ptr<ResolvedBlock> body;

        ResolvedFunctionDecl(SourceLocation location,
                             std::string identifier,
                             Type type,
                             std::vector<std::unique_ptr<ResolvedParamDecl>> params,
                             std::unique_ptr<ResolvedBlock> body)
                             : ResolvedDecl(location,std::move(identifier),type),
                             params(std::move(params)),
                             body(std::move(body)){}
        void dump(size_t level=0)const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct ResolvedCallExpr:public ResolvedExpr{
        const ResolvedFunctionDecl *callee;
        std::vector<std::unique_ptr<ResolvedExpr>> arguments;

        ResolvedCallExpr(SourceLocation location,
                         const ResolvedFunctionDecl &callee,
                         std::vector<std::unique_ptr<ResolvedExpr>> arguments)
                         : ResolvedExpr(location,callee.type),
                         callee(&callee),
                         arguments(std::move(arguments)){}
        void dump(size_t level=0)const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };

    struct ResolvedReturnStmt:public ResolvedStmt{
        std::unique_ptr<ResolvedExpr> expr;

        ResolvedReturnStmt(SourceLocation location,std::unique_ptr<ResolvedExpr> expr= nullptr)
        : ResolvedStmt(location),
        expr(std::move(expr)){}
        void dump(size_t level=0)const override;
        std::string indent(size_t level) const {return std::string(level*2,' ');}
    };





}
