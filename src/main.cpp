#include <iostream>
#include "lexer.hpp"

int main()
{
    std::vector<std::string> cases = {"<", ">", "!", "!=", "<="};
    for (std::string s : cases)
    {
        Lexer lex(s);
        std::vector<Token> tokenList;
        try
        {
            tokenList = lex.tokenize();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
        for (const auto &t : tokenList)
        {
            t.printT();
        }
    }

    return 0;
}