#pragma once

#include <vector>
#include "token.hpp"
#include <cctype>
#include <stdexcept>
#include <iomanip>

class Lexer
{
public:
    Lexer(const std::string &src);

    std::vector<Token> tokenize();

private:
    std::string src;
    int i = 0; // index
    std::string buffer;

    char peek();
    void consume();
};