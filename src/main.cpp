#include <iostream>
#include "lexer.hpp"

int main()
{
    Lexer lex("integer RET ret print false");
    std::vector<Token> tokenList;
    try
    {
        tokenList = lex.tokenize();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
    for (const auto &t : tokenList)
    {
        t.printT();
    }
    return 0;
}