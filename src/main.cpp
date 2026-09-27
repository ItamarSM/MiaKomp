#include <iostream>
#include "lexer.hpp"

int main()
{
    std::vector<std::string> cases = {"\"hello\"", "\"\"", "\"a\\nb\"", "\"say \\\"hi\\\"\"", "\"back\\\\slash\"", "\"tab\\there\"", "\"#not a comment\"", "make s:string = \"x\";", "\"abc", "\"abc\\", "\"a\\qb"};
    for (const auto &s : cases)
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
            std::cout << "size: " << t.text.size() << std::endl;
        }
        std::cout << "\n\n----------------\n\n";
    }

    return 0;
}