#include <iostream>
#include "lexer.hpp"
#include <fstream>
#include <sstream>

void printUsage()
{
    std::cout << "Usage:\n"
                 "For a Debug run- './build/Debug/miakomp <filename>.miak'\n"
                 "For a Real run - './build/Release/miakomp <filename>.miak'\n";

    exit(EXIT_FAILURE);
}

std::string readFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file)
    {
        throw std::runtime_error("Cannot open file '" + path + "'");
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printUsage();
    }

    std::string sourceCode;
    std::string filePath = argv[1];
    try
    {
        sourceCode = readFile(filePath);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    Lexer lex(sourceCode);
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
        std::cout << "size: " << t.text.size() << std::endl;
    }

    return 0;
}