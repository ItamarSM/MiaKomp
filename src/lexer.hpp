#pragma once

#include <vector>
#include "token.hpp"
#include <cctype>
#include <stdexcept>
#include <iomanip>
#include <unordered_map>

class Lexer
{
public:
    Lexer(const std::string &src);

    std::vector<Token> tokenize();

private:
    std::string src;
    int i = 0; // index
    std::string buffer;

    std::unordered_map<char, TokenType> opMap{
        {'(', TokenType::PARAN_OPEN},
        {')', TokenType::PARAN_CLOSE},
        {'{', TokenType::BRACE_OPEN},
        {'}', TokenType::BRACE_CLOSE},
        {';', TokenType::SEMI},
        {',', TokenType::COMMA},
        {':', TokenType::COLUMN},
        {'+', TokenType::PLUS_OP},
        {'-', TokenType::SUB_OP},
        {'*', TokenType::MUL_OP},
        {'/', TokenType::DIV_OP}};

    char peek();
    char peekAhead(int c);
    void consume();
};