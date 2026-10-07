#pragma once

#include "token.hpp"
#include "ast.hpp"

class Parser
{
public:
    Parser(std::vector<Token> tokens);
    std::unique_ptr<Expr> parseExpr();

private:
    const Token &peek();
    const Token &advance();
    bool check(TokenType type) const;
    void expect(TokenType type, std::string message);

    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parseMul();
    std::unique_ptr<Expr> parseAdd();
    std::unique_ptr<Expr> parseCompare();
    std::unique_ptr<Expr> parseOne();

private:
    const std::vector<Token> tokens;
    int pos = 0;
};