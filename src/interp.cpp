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