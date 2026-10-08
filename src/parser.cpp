#include "parser.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(tokens)
{
}

const Token &Parser::peek()
{
    return this->tokens[this->pos];
}

const Token &Parser::peekAhead(int c)
{
    return this->tokens[this->pos + c];
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

const Token &Parser::expect(TokenType type, std::string message)
{
    if (this->tokens[this->pos].type != type)
    {
        throw std::runtime_error(message);
    }
    return this->advance();
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
    else if (tok.type == TokenType::IDENT)
    {
        std::string name = tok.text;
        if (peek().type != TokenType::PARAN_OPEN)
        {
            auto var = std::make_unique<Expr>();
            var->kind = Var{name};
            return var;
        }
        advance();
        std::vector<std::unique_ptr<Expr>> args;
        if (peek().type == TokenType::PARAN_CLOSE)
        {
            auto func = std::make_unique<Expr>();
            func->kind = Call{name, std::move(args)};
            advance();
            return func;
        }
        args.push_back(this->parseOne());
        while (peek().type == TokenType::COMMA)
        {
            advance();
            args.push_back(this->parseOne());
        }
        expect(TokenType::PARAN_CLOSE, "Expected ')'");
        auto fn = std::make_unique<Expr>();
        fn->kind = Call{name, std::move(args)};
        return fn;
    }
    else if (tok.type == TokenType::PARAN_OPEN)
    {
        auto var = parseOne();
        expect(TokenType::PARAN_CLOSE, "Expected ')'");
        return var;
    }
    throw std::runtime_error("Expected a value, got" + tok.text);
}

std::unique_ptr<Expr> Parser::parseUnary()
{
    if (peek().type == TokenType::SUB_OP)
    {
        advance();
        auto var = parseUnary();
        auto neg = std::make_unique<Expr>();
        neg->kind = Unary{UnaryOp::NEG, std::move(var)};
        return neg;
    }
    return parsePrimary();
}

std::unique_ptr<Expr> Parser::parseMul()
{
    auto left = parseUnary();
    while (peek().type == TokenType::MUL_OP || peek().type == TokenType::DIV_OP)
    {
        auto type = advance().type;
        if (type == TokenType::MUL_OP)
        {
            auto right = parseUnary();
            auto expr = std::make_unique<Expr>();
            expr->kind = Binary{BinaryOp::MUL, std::move(left), std::move(right)};
            left = std::move(expr);
        }
        else if (type == TokenType::DIV_OP)
        {
            auto right = parseUnary();
            auto expr = std::make_unique<Expr>();
            expr->kind = Binary{BinaryOp::DIV, std::move(left), std::move(right)};
            left = std::move(expr);
        }
    }
    return left;
}

std::unique_ptr<Expr> Parser::parseAdd()
{
    auto left = parseMul();
    while (peek().type == TokenType::PLUS_OP || peek().type == TokenType::SUB_OP)
    {
        auto type = advance().type;
        if (type == TokenType::PLUS_OP)
        {
            auto right = parseMul();
            auto expr = std::make_unique<Expr>();
            expr->kind = Binary{BinaryOp::ADD, std::move(left), std::move(right)};
            left = std::move(expr);
        }
        else if (type == TokenType::SUB_OP)
        {
            auto right = parseMul();
            auto expr = std::make_unique<Expr>();
            expr->kind = Binary{BinaryOp::SUB, std::move(left), std::move(right)};
            left = std::move(expr);
        }
    }
    return left;
}

std::unique_ptr<Expr> Parser::parseCompare()
{
    auto first = parseAdd();
    auto type = peek().type;
    if (type == TokenType::GT_COMP || type == TokenType::GTEQ_COMP || type == TokenType::LT_COMP || type == TokenType::LTEQ_COMP || type == TokenType::EQ_COMP || type == TokenType::NOT_COMP)
    {
        std::vector<std::unique_ptr<Expr>> operands;
        std::vector<CompareOp> ops;
        operands.push_back(std::move(first));
        do
        {
            advance();
            if (type == TokenType::GT_COMP)
            {
                ops.push_back(CompareOp::GT);
            }
            else if (type == TokenType::GTEQ_COMP)
            {
                ops.push_back(CompareOp::GTEQ);
            }
            else if (type == TokenType::LT_COMP)
            {
                ops.push_back(CompareOp::LT);
            }
            else if (type == TokenType::LTEQ_COMP)
            {
                ops.push_back(CompareOp::LTEQ);
            }
            else if (type == TokenType::EQ_COMP)
            {
                ops.push_back(CompareOp::EQ);
            }
            else if (type == TokenType::NOT_COMP)
            {
                ops.push_back(CompareOp::NOTEQ);
            }
            operands.push_back(parseAdd());
            type = peek().type;

        } while (type == TokenType::GT_COMP || type == TokenType::GTEQ_COMP || type == TokenType::LT_COMP || type == TokenType::LTEQ_COMP || type == TokenType::EQ_COMP || type == TokenType::NOT_COMP);
        auto expr = std::make_unique<Expr>();
        expr->kind = Compare{std::move(operands), ops};
        return expr;
    }
    else
    {
        return first;
    }
}

std::unique_ptr<Expr> Parser::parseNot()
{
    if (peek().type == TokenType::not_KW)
    {
        advance();
        auto var = parseNot();
        auto neg = std::make_unique<Expr>();
        neg->kind = Unary{UnaryOp::NOT, std::move(var)};
        return neg;
    }
    return parseCompare();
}

std::unique_ptr<Expr> Parser::parseAnd()
{
    auto left = parseNot();
    while (peek().type == TokenType::and_KW)
    {
        advance();
        auto right = parseNot();
        auto expr = std::make_unique<Expr>();
        expr->kind = Binary{BinaryOp::AND, std::move(left), std::move(right)};
        left = std::move(expr);
    }
    return left;
}

std::unique_ptr<Expr> Parser::parseOr()
{
    auto left = parseAnd();
    while (peek().type == TokenType::or_KW)
    {
        advance();
        auto right = parseAnd();
        auto expr = std::make_unique<Expr>();
        expr->kind = Binary{BinaryOp::OR, std::move(left), std::move(right)};
        left = std::move(expr);
    }
    return left;
}

std::unique_ptr<Expr> Parser::parseOne()
{
    auto var = parseOr();
    return var;
}

std::unique_ptr<Expr> Parser::parseExpr()
{
    auto var = parseOne();
    expect(TokenType::END, "Expected the file to end");
    return var;
}

Type Parser::parseType()
{
    auto type = advance().type;
    if (type == TokenType::BOOL_KW)
    {
        return Type::BOOL;
    }
    else if (type == TokenType::STRING_KW)
    {
        return Type::STRING;
    }
    else if (type == TokenType::FLOAT_KW)
    {
        return Type::FLOAT;
    }
    else if (type == TokenType::INT_KW)
    {
        return Type::INT;
    }
    else
    {
        throw std::runtime_error("Expected a type");
    }
}

std::vector<std::unique_ptr<Stmt>> Parser::parseBlock()
{
    expect(TokenType::BRACE_OPEN, "Expected '{'");
    std::vector<std::unique_ptr<Stmt>> block;
    while (!check(TokenType::BRACE_CLOSE))
    {
        if (peek().type == TokenType::END)
        {
            throw std::runtime_error("Expected '}'");
        }
        block.push_back(parseStmt());
    }
    advance();
    return block;
}

std::unique_ptr<Stmt> Parser::parseStmt()
{
    auto tok = peek();
    if (tok.type == TokenType::make_KW)
    {
        advance();
        auto name = expect(TokenType::IDENT, "Invalid name").text;
        expect(TokenType::COLUMN, "Expected ':'");
        auto type = parseType();
        expect(TokenType::EQ_OP, "Expected '='");
        auto value = parseOne();
        expect(TokenType::SEMI, "Expected ';'");
        auto var = std::make_unique<Stmt>();
        var->stmt = Make{name, type, std::move(value)};
        return var;
    }
    else if (tok.type == TokenType::RET_KW)
    {
        advance();
        auto val = parseOne();
        expect(TokenType::SEMI, "Expected ';'");
        auto var = std::make_unique<Stmt>();
        var->stmt = Ret{std::move(val)};
        return var;
    }
    else if (tok.type == TokenType::if_KW)
    {
        advance();
        auto condition = parseOne();
        auto body = parseBlock();
        std::vector<std::unique_ptr<Stmt>> elseBody;
        if (peek().type == TokenType::else_KW)
        {
            advance();
            elseBody = parseBlock();
        }
        auto var = std::make_unique<Stmt>();
        var->stmt = If{std::move(condition), std::move(body), std::move(elseBody)};
        return var;
    }
    else if (tok.type == TokenType::while_KW)
    {
        advance();
        auto condition = parseOne();
        auto body = parseBlock();
        auto var = std::make_unique<Stmt>();
        var->stmt = While{std::move(condition), std::move(body)};
        return var;
    }
    else if (tok.type == TokenType::IDENT && peekAhead(1).type == TokenType::EQ_OP)
    {
        advance();
        advance();
        auto name = tok.text;
        auto value = parseOne();
        expect(TokenType::SEMI, "Expected ';'");
        auto var = std::make_unique<Stmt>();
        var->stmt = Assign{name, std::move(value)};
        return var;
    }
    else
    {
        auto val = parseOne();
        expect(TokenType::SEMI, "Expected ';'");
        auto expr = std::make_unique<Stmt>();
        expr->stmt = ExprStmt{std::move(val)};
        return expr;
    }
}

std::unique_ptr<Stmt> Parser::parseOneStmt()
{
    auto var = parseStmt();
    expect(TokenType::END, "Expected the file to end");
    return var;
}