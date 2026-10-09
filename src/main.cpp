#include <iostream>
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
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

std::string binaryToString(const BinaryOp &e)
{
    switch (e)
    {
    case BinaryOp::ADD:
        return " + ";
        break;
    case BinaryOp::SUB:
        return " - ";
        break;
    case BinaryOp::MUL:
        return " * ";
        break;
    case BinaryOp::DIV:
        return " / ";
        break;
    case BinaryOp::AND:
        return " and ";
        break;
    case BinaryOp::OR:
        return " or ";
        break;
    };
    throw std::runtime_error("no such op");
}

std::string unaryToString(const UnaryOp &e)
{
    switch (e)
    {
    case UnaryOp::NEG:
        return "-";
        break;
    case UnaryOp::NOT:
        return "not ";
        break;
    };
    throw std::runtime_error("no such op");
}

std::string compareToString(const CompareOp &e)
{
    switch (e)
    {
    case CompareOp::EQ:
        return " =? ";
        break;
    case CompareOp::GT:
        return " >? ";
        break;
    case CompareOp::GTEQ:
        return " >=? ";
        break;
    case CompareOp::LT:
        return " <? ";
        break;
    case CompareOp::LTEQ:
        return " <=? ";
        break;
    case CompareOp::NOTEQ:
        return " !=? ";
        break;
    };
    throw std::runtime_error("no such op");
}

class Printer
{
public:
    void operator()(const IntLit &e) { std::cout << e.value; }
    void operator()(const FloatLit &e) { std::cout << e.value; }
    void operator()(const BoolLit &e) { std::cout << (e.value == true ? "true" : "false"); }
    void operator()(const StringLit &e) { std::cout << e.value; }
    void operator()(const Var &e) { std::cout << e.name; }
    void operator()(const Unary &e)
    {
        std::cout << unaryToString(e.op);
        std::visit(Printer{}, e.expr->kind);
    }
    void operator()(const Binary &e)
    {
        std::cout << "(";
        std::visit(Printer{}, e.left->kind);
        std::cout << binaryToString(e.op);
        std::visit(Printer{}, e.right->kind);
        std::cout << ")";
    }
    void operator()(const Compare &e)
    {
        std::cout << "(";
        for (size_t i = 0; i < e.operands.size() - 1; i++)
        {
            const std::unique_ptr<Expr> &operand = e.operands[int(i)];
            std::visit(Printer{}, operand->kind);
            auto op = e.ops[i];
            std::cout << compareToString(op);
        }
        std::visit(Printer{}, e.operands.back()->kind);
        std::cout << ")";
    }
    void operator()(const Call &e)
    {
        std::cout << "Call, name : " << e.name << "\n";
        int i = 0;
        for (const std::unique_ptr<Expr> &arg : e.args)
        {
            std::cout << "arg " << i << ": ";
            std::visit(Printer{}, arg->kind);
            std::cout << "\n";
            i++;
        }
    }
};

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
    /*
    for (const auto &t : tokenList)
    {
        t.printT();
        std::cout << "size: " << t.text.size() << std::endl;
    }
    */

    Parser parser(tokenList);
    std::unique_ptr<Expr> prog;
    try
    {
        prog = parser.parseExpr();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    std::visit(Printer{}, prog->kind);
    std::cout<<std::endl;

    /*

    auto one = std::make_unique<Expr>();
    one->kind = IntLit{1};

    auto two = std::make_unique<Expr>();
    two->kind = IntLit{2};

    auto three = std::make_unique<Expr>();
    three->kind = IntLit{3};

    auto right = std::make_unique<Expr>();
    right->kind = Binary{BinaryOp::MUL, std::move(two), std::move(three)};

    auto top = std::make_unique<Expr>();
    top->kind = Binary{BinaryOp::ADD, std::move(one), std::move(right)};

    std::visit(Printer{}, top->kind);
    std::cout << "\n";

    auto a = std::make_unique<Expr>();
    a->kind = Var{"a"};

    auto b = std::make_unique<Expr>();
    b->kind = Var{"b"};

    auto c = std::make_unique<Expr>();
    c->kind = Var{"c"};

    std::vector<std::unique_ptr<Expr>> operands;
    operands.push_back(std::move(a));
    operands.push_back(std::move(b));
    operands.push_back(std::move(c));

    std::vector<CompareOp> ops;
    ops.push_back(CompareOp::LT);
    ops.push_back(CompareOp::LT);

    auto compare = std::make_unique<Expr>();
    compare->kind = Compare{std::move(operands), ops};

    std::visit(Printer{}, compare->kind);
    std::cout << "\n";

    */

    return 0;
}