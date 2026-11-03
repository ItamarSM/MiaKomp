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

void Env::pushScope()
{
    std::unordered_map<std::string, Value> scopeVar{};
    this->scopeMap.push_back(scopeVar);
}

void Env::popScope()
{
    if (!this->scopeMap.empty())
    {
        this->scopeMap.pop_back();
    }
    else
    {
        throw std::runtime_error("Cannot pop from an empty frame");
    }
}

void Env::declare(std::string name, const Value &e)
{
    if (this->globMap.find(name) == this->globMap.end() && !checkKey(this->scopeMap, name))
    {
        if (scopeMap.size() != 0)
        {
            scopeMap.back()[name] = e;
        }
        else
        {
            globMap[name] = e;
        }
    }
    else
    {
        throw std::runtime_error("Variable name already used");
    }
}

Value &Env::lookup(std::string name)
{
    for (int i = int(this->scopeMap.size()) - 1; i >= 0; i--)
    {
        if (this->scopeMap[i].find(name) != this->scopeMap[i].end())
        {
            return this->scopeMap[i][name];
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
Value Evaluator::operator()(const Compare &e) { throw std::runtime_error("not implemented"); }
Value Evaluator::operator()(const Call &e) { throw std::runtime_error("not implemented"); }