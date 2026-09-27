#include "lexer.hpp"

Lexer::Lexer(const std::string &src)
{
    this->src = src;
};

char Lexer::peek()
{
    return this->src[this->i];
}

void Lexer::consume()
{
    this->buffer += this->src[this->i];
    i++;
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> ret;
    while (peek())
    {
        if (peek() == '_' || std::isalpha(static_cast<unsigned char>(peek())))
        {
            while (std::isalpha(static_cast<unsigned char>(peek())) || std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
            {
                consume();
            }
            if (buffer == "int")
            {
                Token t;
                t.type = TokenType::INT_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "float")
            {
                Token t;
                t.type = TokenType::FLOAT_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "string")
            {
                Token t;
                t.type = TokenType::STRING_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "bool")
            {
                Token t;
                t.type = TokenType::BOOL_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "make")
            {
                Token t;
                t.type = TokenType::make_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "fn")
            {
                Token t;
                t.type = TokenType::fn_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "RET")
            {
                Token t;
                t.type = TokenType::RET_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "if")
            {
                Token t;
                t.type = TokenType::if_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "else")
            {
                Token t;
                t.type = TokenType::else_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "while")
            {
                Token t;
                t.type = TokenType::while_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "main")
            {
                Token t;
                t.type = TokenType::main_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "and")
            {
                Token t;
                t.type = TokenType::and_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "not")
            {
                Token t;
                t.type = TokenType::not_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "or")
            {
                Token t;
                t.type = TokenType::or_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "true")
            {
                Token t;
                t.type = TokenType::true_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else if (buffer == "false")
            {
                Token t;
                t.type = TokenType::false_KW;
                ret.push_back(t);
                buffer.clear();
            }
            else
            {
                Token t;
                t.type = TokenType::IDENT;
                t.text = buffer;
                ret.push_back(t);
                buffer.clear();
            }
        }
        else if (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n')
        {
            consume();
            this->buffer.clear();
            continue;
        }
        else if (peek() == '#')
        {
            while (i < this->src.size() && peek() != '\n')
            {
                consume();
            }
            if (peek() == '\n')
                consume();
            this->buffer.clear();
        }
        else
        {
            throw std::runtime_error("unknown char");
        }
    }
    Token end;
    end.type = TokenType::END;
    ret.push_back(end);
    return ret;
}