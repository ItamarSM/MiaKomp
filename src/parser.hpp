#pragma once

#include "token.hpp"
#include "ast.hpp"

class Parser
{
public:
    Parser(std::vector<Token> tokens);
    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<Stmt> parseOneStmt();
    Program parseProgram();

private:
    const Token &peek();
    const Token &peekAhead(int c);
    const Token &advance();
    bool check(TokenType type) const;
    const Token &expect(TokenType type, std::string message);

    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parseMul();
    std::unique_ptr<Expr> parseAdd();
    std::unique_ptr<Expr> parseCompare();
    std::unique_ptr<Expr> parseNot();
    std::unique_ptr<Expr> parseAnd();
    std::unique_ptr<Expr> parseOr();
    std::unique_ptr<Expr> parseOne();

    std::unique_ptr<Stmt> parseStmt();

    Make parseMake();
    Param parseParam();
    FnDecl parseFn();
    bool checkParam(const Param &p, bool &d);
    std::vector<std::unique_ptr<Stmt>> parseMain();

    Type parseType();
    std::vector<std::unique_ptr<Stmt>> parseBlock();

private:
    const std::vector<Token> tokens;
    int pos = 0;
};