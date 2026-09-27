#include <iostream>
#include "lexer.hpp"

int main()
{
    Lexer lex("99999999999999999999");
    std::vector<Token> tokenList;
    try
    {
        tokenList = lex.tokenize();
    }
    catch (const std::exception &e)
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