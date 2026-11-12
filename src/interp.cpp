#include "interp.hpp"

std::string valuetoString(const Value &e)
{
    if (std::holds_alternative<std::string>(e))
    {
        return std::get<std::string>(e);
    }
    else if (std::holds_alternative<long long>(e))
    {
        return std::to_string(std::get<long long>(e));
    }
    else if (std::holds_alternative<double>(e))
    {
        std::ostringstream stream;
        stream << std::setprecision(15) << std::get<double>(e);
        return stream.str();
    }
    else if (std::holds_alternative<bool>(e))
    {
        return std::get<bool>(e) ? "true" : "false";
    }
    else if (std::holds_alternative<std::monostate>(e))
    {
        return "";
    }
    return "";
}

void Env::pushFrame()
{
    std::vector<std::unordered_map<std::string, Value>> scopeVar{};
    this->frameMap.push_back(scopeVar);
    pushScope();
}

void Env::popFrame()
{
    if (!this->frameMap.empty())
    {
        this->frameMap.pop_back();
    }
    else
    {
        throw std::runtime_error("Cannot pop from an empty frame");
    }
}

void Env::pushScope()
{
    std::unordered_map<std::string, Value> scopeVar{};
    this->frameMap.back().push_back(scopeVar);
}

void Env::popScope()
{
    if (!this->frameMap.back().empty())
    {
        this->frameMap.back().pop_back();
    }
    else
    {
        throw std::runtime_error("Cannot pop from an empty scope");
    }
}

void Env::declareFn(const FnDecl &fn)
{
    if (fnMap.end() == fnMap.find(fn.name))
    {
        fnMap[fn.name] = &fn;
    }
    else
    {
        throw std::runtime_error("function name already used");
    }
}

const FnDecl &Env::lookupFn(const std::string name)
{
    if (fnMap.end() != fnMap.find(name))
    {
        return *fnMap[name];
    }
    else
    {
        throw std::runtime_error("no such ifunction name");
    }
}

void Env::declare(std::string name, const Value &e)
{
    if (globMap.end() != globMap.find(name) || (frameMap.size() > 0 && checkKey(frameMap.back(), name)))
    {
        throw std::runtime_error("Variable name already used");
    }
    else if (frameMap.size() > 0)
    {
        frameMap.back().back()[name] = e;
    }
    else
    {
        globMap[name] = e;
    }
}

Value &Env::lookup(std::string name)
{
    if (frameMap.size() > 0)
    {
        for (int i = int(this->frameMap.back().size()) - 1; i >= 0; i--)
        {
            if (this->frameMap.back()[i].find(name) != this->frameMap.back()[i].end())
            {
                return this->frameMap.back()[i][name];
            }
        }
    }
    if (this->globMap.find(name) != this->globMap.end())
    {
        return this->globMap[name];
    }
    else
    {
        throw std::runtime_error("Undefined variable");
    }
}

void Env::assign(std::string name, const Value &e)
{
    this->lookup(name) = e;
}

bool checkKey(const std::vector<std::unordered_map<std::string, Value>> &e, std::string name)
{

    for (int i = int(e.size()) - 1; i >= 0; i--)
    {
        if (e[i].find(name) != e[i].end())
        {
            return true;
        }
    }

    return false;
}

Evaluator::Evaluator(Env &env) : env(env)
{
}

Value Evaluator::operator()(const IntLit &e)
{
    return e.value;
}
Value Evaluator::operator()(const FloatLit &e)
{
    return e.value;
}
Value Evaluator::operator()(const StringLit &e)
{
    return e.value;
}
Value Evaluator::operator()(const BoolLit &e)
{
    return e.value;
}
Value Evaluator::operator()(const Var &e)
{
    return this->env.lookup(e.name);
}
Value Evaluator::operator()(const Unary &e)
{
    Value eval = std::visit(*this, e.expr->kind);
    switch (e.op)
    {
    case UnaryOp::NEG:
        if (std::holds_alternative<long long>(eval))
        {
            return Value{-1 * std::get<long long>(eval)};
        }
        if (std::holds_alternative<double>(eval))
        {
            return Value{-1 * std::get<double>(eval)};
        }
        throw std::runtime_error("Cannot negate this");
    case UnaryOp::NOT:
        if (std::holds_alternative<bool>(eval))
        {
            return !std::get<bool>(eval);
        }
        throw std::runtime_error("Cannot negate this");
    default:
        throw std::runtime_error("not such op");
    }
}
Value Evaluator::operator()(const Binary &e)
{
    Value l = std::visit(*this, e.left->kind);

    bool andOr = 0;
    if (e.op == BinaryOp::AND || e.op == BinaryOp::OR)
    {
        if (std::holds_alternative<bool>(l))
        {
            if (!std::get<bool>(l) && e.op == BinaryOp::AND)
            {
                return false;
            }
            else if (std::get<bool>(l) && e.op == BinaryOp::OR)
            {
                return true;
            }
            else
            {
                andOr = 1;
            }
        }
        else
        {
            throw std::runtime_error("operands must be of type bool");
        }
    }

    Value r = std::visit(*this, e.right->kind);

    if (andOr)
    {
        if (std::holds_alternative<bool>(r))
        {
            return std::get<bool>(r);
        }
        else
        {
            throw std::runtime_error("operands must be of type bool");
        }
    }

    switch (e.op)
    {
    case BinaryOp::ADD:
        if (std::holds_alternative<long long>(l) && std::holds_alternative<long long>(r))
        {
            return std::get<long long>(l) + std::get<long long>(r);
        }
        else if (std::holds_alternative<long long>(l) && std::holds_alternative<double>(r))
        {
            return double(std::get<long long>(l) + std::get<double>(r));
        }
        else if (std::holds_alternative<double>(l) && std::holds_alternative<long long>(r))
        {
            return double(std::get<double>(l) + std::get<long long>(r));
        }
        else if (std::holds_alternative<double>(l) && std::holds_alternative<double>(r))
        {
            return double(std::get<double>(l) + std::get<double>(r));
        }
        else if (std::holds_alternative<std::string>(l) && std::holds_alternative<std::string>(r))
        {
            return std::get<std::string>(l) + std::get<std::string>(r);
        }
        else
        {
            throw std::runtime_error("Operand types must be int/float");
        }

    case BinaryOp::SUB:
        if (std::holds_alternative<long long>(l) && std::holds_alternative<long long>(r))
        {
            return std::get<long long>(l) - std::get<long long>(r);
        }
        else if (std::holds_alternative<long long>(l) && std::holds_alternative<double>(r))
        {
            return double(std::get<long long>(l) - std::get<double>(r));
        }
        else if (std::holds_alternative<double>(l) && std::holds_alternative<long long>(r))
        {
            return double(std::get<double>(l) - std::get<long long>(r));
        }
        else if (std::holds_alternative<double>(l) && std::holds_alternative<double>(r))
        {
            return double(std::get<double>(l) - std::get<double>(r));
        }
        else
        {
            throw std::runtime_error("Operand types must be int/float");
        }

    case BinaryOp::MUL:
        if (std::holds_alternative<long long>(l) && std::holds_alternative<long long>(r))
        {
            return std::get<long long>(l) * std::get<long long>(r);
        }
        else if (std::holds_alternative<long long>(l) && std::holds_alternative<double>(r))
        {
            return double(std::get<long long>(l) * std::get<double>(r));
        }
        else if (std::holds_alternative<double>(l) && std::holds_alternative<long long>(r))
        {
            return double(std::get<double>(l) * std::get<long long>(r));
        }
        else if (std::holds_alternative<double>(l) && std::holds_alternative<double>(r))
        {
            return double(std::get<double>(l) * std::get<double>(r));
        }
        else
        {
            throw std::runtime_error("Operand types must be int/float");
        }

    case BinaryOp::DIV:
        if (std::holds_alternative<long long>(r))
        {
            if (std::get<long long>(r) == 0LL)
            {
                throw std::runtime_error("cannot divide by 0");
            }
        }
        if (std::holds_alternative<double>(r))
        {
            if (std::get<double>(r) == double(0))
            {
                throw std::runtime_error("cannot divide by 0");
            }
        }
        if (std::holds_alternative<long long>(l) && std::holds_alternative<long long>(r))
        {
            return static_cast<long long>(std::get<long long>(l) / std::get<long long>(r));
        }
        else if (std::holds_alternative<long long>(l) && std::holds_alternative<double>(r))
        {
            return double(std::get<long long>(l) / std::get<double>(r));
        }
        else if (std::holds_alternative<double>(l) && std::holds_alternative<long long>(r))
        {
            return double(std::get<double>(l) / std::get<long long>(r));
        }
        else if (std::holds_alternative<double>(l) && std::holds_alternative<double>(r))
        {
            return double(std::get<double>(l) / std::get<double>(r));
        }
        else
        {
            throw std::runtime_error("Operand types must be int/float");
        }
    }
    throw std::runtime_error("No such binary op");
}
Value Evaluator::operator()(const Compare &e)
{
    Value left = std::visit(*this, e.operands[0]->kind);
    bool cont = true;
    for (int i = 0; i < int(e.ops.size()); i++)
    {
        Value right = std::visit(*this, e.operands[i + 1]->kind);
        switch (e.ops[i])
        {
        case CompareOp::EQ:
            if (std::holds_alternative<long long>(left) && std::holds_alternative<long long>(right))
            {
                cont = std::get<long long>(left) == std::get<long long>(right);
            }
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            {
                cont = std::get<double>(left) == std::get<double>(right);
            }
            else if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
            {
                cont = std::get<bool>(left) == std::get<bool>(right);
            }
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
            {
                cont = std::get<std::string>(left) == std::get<std::string>(right);
            }
            else
            {
                throw std::runtime_error("Cannot compare these types with each other");
            }
            break;

        case CompareOp::GT:
            if (std::holds_alternative<long long>(left) && std::holds_alternative<long long>(right))
            {
                cont = std::get<long long>(left) > std::get<long long>(right);
            }
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            {
                cont = std::get<double>(left) > std::get<double>(right);
            }
            else
            {
                throw std::runtime_error("Cannot compare these types with each other");
            }
            break;

        case CompareOp::LT:
            if (std::holds_alternative<long long>(left) && std::holds_alternative<long long>(right))
            {
                cont = std::get<long long>(left) < std::get<long long>(right);
            }
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            {
                cont = std::get<double>(left) < std::get<double>(right);
            }
            else
            {
                throw std::runtime_error("Cannot compare these types with each other");
            }
            break;

        case CompareOp::GTEQ:
            if (std::holds_alternative<long long>(left) && std::holds_alternative<long long>(right))
            {
                cont = std::get<long long>(left) >= std::get<long long>(right);
            }
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            {
                cont = std::get<double>(left) >= std::get<double>(right);
            }
            else
            {
                throw std::runtime_error("Cannot compare these types with each other");
            }
            break;

        case CompareOp::LTEQ:
            if (std::holds_alternative<long long>(left) && std::holds_alternative<long long>(right))
            {
                cont = std::get<long long>(left) <= std::get<long long>(right);
            }
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            {
                cont = std::get<double>(left) <= std::get<double>(right);
            }
            else
            {
                throw std::runtime_error("Cannot compare these types with each other");
            }
            break;

        case CompareOp::NOTEQ:
            if (std::holds_alternative<long long>(left) && std::holds_alternative<long long>(right))
            {
                cont = std::get<long long>(left) != std::get<long long>(right);
            }
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            {
                cont = std::get<double>(left) != std::get<double>(right);
            }
            else if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
            {
                cont = std::get<bool>(left) != std::get<bool>(right);
            }
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
            {
                cont = std::get<std::string>(left) != std::get<std::string>(right);
            }
            else
            {
                throw std::runtime_error("Cannot compare these types with each other");
            }
            break;
        }
        if (!cont)
        {
            return false;
        }
        left = right;
    }
    return true;
}

Value Evaluator::operator()(const Call &e)
{
    if (e.name == "print")
    {
        for (const std::unique_ptr<Expr> &expr : e.args)
        {
            std::cout << valuetoString(std::visit(*this, expr->kind));
        }
        return std::monostate{};
    }
    const FnDecl &fn = env.lookupFn(e.name);
    unsigned int mandatory = 0;
    for (const Param &p : fn.params)
    {
        if (p.value == nullptr)
        {
            mandatory++;
        }
    }
    if (e.args.size() < mandatory || e.args.size() > fn.params.size())
    {
        throw std::runtime_error("wrong number of args were given");
    }
    std::vector<Value> args;
    for (int i = 0; i < int(e.args.size()); i++)
    {
        const std::unique_ptr<Expr> &p = e.args[i];
        Value pVal = std::visit(Evaluator{*this}, p->kind);
        if (fn.params[i].type == Type::FLOAT && std::holds_alternative<long long>(pVal))
        {
            pVal = double(std::get<long long>(pVal));
        }
        args.push_back(pVal);
    }
    env.pushFrame();
    for (int i = 0; i < int(fn.params.size()); i++)
    {
        if (i < int(args.size()))
        {
            env.declare(fn.params[i].name, args[i]);
        }
        else
        {
            Value defVal = std::visit(Evaluator{*this}, fn.params[i].value->kind);
            if (fn.params[i].type == Type::FLOAT && std::holds_alternative<long long>(defVal))
            {
                defVal = double(std::get<long long>(defVal));
            }
            env.declare(fn.params[i].name, defVal);
        }
    }
    for (const std::unique_ptr<Stmt> &s : fn.body)
    {
        auto ret = std::visit(Executor{env}, s->stmt);
        if (ret.has_value())
        {
            env.popFrame();
            if (fn.retType == Type::FLOAT && std::holds_alternative<long long>(ret.value()))
            {
                ret = double(std::get<long long>(ret.value()));
            }
            return ret.value();
        }
    }
    throw std::runtime_error("function ended without return");
}

Executor::Executor(Env &env) : env(env) {}

std::optional<Value> Executor::operator()(const Make &e)
{
    Value val = std::visit(eval, e.value->kind);
    if (e.type == Type::FLOAT && std::holds_alternative<long long>(val))
    {
        val = double(std::get<long long>(val));
    }
    env.declare(e.name, val);
    return {};
}
std::optional<Value> Executor::operator()(const Assign &e)
{
    Value newVal = std::visit(eval, e.value->kind);
    if (std::holds_alternative<double>(env.lookup(e.name)) && std::holds_alternative<long long>(newVal))
    {
        newVal = double(std::get<long long>(newVal));
    }
    env.assign(e.name, newVal);
    return {};
}
std::optional<Value> Executor::operator()(const ExprStmt &e)
{
    Value a = std::visit(eval, e.expr->kind);
    return {};
}
std::optional<Value> Executor::operator()(const If &e)
{
    Value condition = std::visit(eval, e.condition->kind);
    if (!std::holds_alternative<bool>(condition))
    {
        throw std::runtime_error("condition must be of type bool");
    }
    if (std::get<bool>(condition))
    {
        env.pushScope();
        for (const std::unique_ptr<Stmt> &stmt : e.body)
        {
            auto ret = std::visit(*this, stmt->stmt);
            if (ret.has_value())
            {
                env.popScope();
                return ret;
            }
        }
        env.popScope();
    }
    else
    {
        env.pushScope();
        for (const std::unique_ptr<Stmt> &stmt : e.elseBody)
        {
            auto ret = std::visit(*this, stmt->stmt);
            if (ret.has_value())
            {
                env.popScope();
                return ret;
            }
        }
        env.popScope();
    }
    return {};
}
std::optional<Value> Executor::operator()(const While &e)
{
    Value condition = std::visit(eval, e.condition->kind);
    if (!std::holds_alternative<bool>(condition))
    {
        throw std::runtime_error("condition must be of type bool");
    }
    while (std::get<bool>(condition))
    {
        env.pushScope();
        for (const std::unique_ptr<Stmt> &stmt : e.body)
        {
            auto ret = std::visit(*this, stmt->stmt);
            if (ret.has_value())
            {
                env.popScope();
                return ret;
            }
        }
        env.popScope();
        condition = std::visit(eval, e.condition->kind);
    }
    return {};
}
std::optional<Value> Executor::operator()(const Ret &e)
{
    return std::visit(eval, e.value->kind);
}

int runProgram(const Program &prog)
{
    Env env;
    Evaluator eval(env);
    Executor exec(env);

    for (const FnDecl &fn : prog.funcs)
    {
        env.declareFn(fn);
    }
    for (const Make &g : prog.globals)
    {
        Value glob = std::visit(eval, g.value->kind);
        if (g.type == Type::FLOAT && std::holds_alternative<long long>(glob))
        {
            glob = double(std::get<long long>(glob));
        }
        env.declare(g.name, glob);
    }
    env.pushFrame();
    for (const std::unique_ptr<Stmt> &s : prog.mainFunc)
    {
        auto ret = std::visit(exec, s->stmt);
        if (ret.has_value())
        {
            env.popFrame();
            if (std::holds_alternative<long long>(ret.value()))
            {
                return int(std::get<long long>(ret.value()));
            }
            else
            {
                throw std::runtime_error("return type must be int");
            }
        }
    }
    throw std::runtime_error("must return an int");
}