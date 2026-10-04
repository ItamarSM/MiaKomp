#include "parser.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(tokens)
{
}

const Token &Parser::peek()
{
    return this->tokens[this->pos];
}

const Token &Parser::advance()
{
    this->pos += 1;
    return this->tokens[this->pos - 1];
}

bool Parser::check(TokenType type) const
{
    return this->tokens[this->pos].type == type;
}

void Parser::expect(TokenType type, std::string message)
{
    if (this->tokens[this->pos].type != type)
    {
        throw std::runtime_error(message);
    }
    this->advance();
}

std::unique_ptr<Expr> Parser::parsePrimary()
{
    const Token &tok = advance();
    if (tok.type == TokenType::int_LIT)
    {
        auto val = std::make_unique<Expr>();
        val->kind = IntLit{tok.int_val};
        return val;
    }
    else if (tok.type == TokenType::string_LIT)
    {
        auto val = std::make_unique<Expr>();
        val->kind = StringLit{tok.text};
        return val;
    }
    else if (tok.type == TokenType::float_LIT)
    {
        auto val = std::make_unique<Expr>();
        val->kind = FloatLit{tok.float_val};
        return val;
    }
    else if (tok.type == TokenType::true_KW || tok.type == TokenType::false_KW)
    {
        auto val = std::make_unique<Expr>();
        val->kind = BoolLit{tok.type == TokenType::true_KW ? true : false};
        return val;
    }
    throw std::runtime_error("Expected a value, got" + tok.text);
}

std::unique_ptr<Expr> Parser::parseExpr()
{
    auto var = parsePrimary();
    expect(TokenType::END, "Expected the file to end");
    return var;
}