#pragma once

#include <string>
#include <memory>
#include <variant>
#include <vector>

enum class BinaryOp
{
    ADD,
    SUB,
    MUL,
    DIV,
    AND,
    OR
};

enum class CompareOp
{
    EQ,
    NOTEQ,
    GT,
    GTEQ,
    LT,
    LTEQ
};

enum class UnaryOp
{
    NEG,
    NOT
};

struct Expr;

struct IntLit
{
    long long value;
};
struct FloatLit
{
    double value;
};
struct BoolLit
{
    bool value;
};
struct StringLit
{
    std::string value;
};
struct Var
{
    std::string name;
};
struct Unary
{
    UnaryOp op;
    std::unique_ptr<Expr> expr;
};
struct Binary
{
    BinaryOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};
struct Compare
{
    std::vector<std::unique_ptr<Expr>> operands;
    std::vector<CompareOp> ops;
};
struct Call
{
    std::string name;
    std::vector<std::unique_ptr<Expr>> args;
};

struct Expr
{
    std::variant<IntLit,
                 FloatLit,
                 BoolLit,
                 StringLit,
                 Var,
                 Unary,
                 Binary,
                 Compare,
                 Call>
        kind;
};

enum class Type
{
    INT,
    FLOAT,
    BOOL,
    STRING
};

struct Make
{
    std::string name;
    Type type;
    std::unique_ptr<Expr> value;
};

struct Assign
{
    std::string name;
    std::unique_ptr<Expr> value;
};

struct Stmt;

struct ExprStmt
{
    std::unique_ptr<Expr> expr;
};

struct If
{
    Expr condition;
    std::vector<std::unique_ptr<Stmt>> body;
    std::vector<std::unique_ptr<Stmt>> elseBody;
};

struct While
{
    Expr condition;
    std::vector<std::unique_ptr<Stmt>> body;
};

struct Ret
{
    Expr value;
};

struct Stmt
{
    std::variant<Make, Assign, ExprStmt, If, While, Ret> stmt;
};

struct Param
{
    std::string name;
    Type type;
    std::variant<long long, double, bool, std::string> value = nullptr;
};