#include "typechecker.hpp"

void TEnv::pushScope()
{
    std::unordered_map<std::string, Type> scope;
    scopeMap.push_back(scope);
}

void TEnv::popScope()
{
    if (scopeMap.size() <= 0)
    {
        throw std::runtime_error("Cannot pop an empty scope map");
    }
    scopeMap.pop_back();
}

Type TEnv::lookup(std::string name)
{
    if (isVisible(name))
    {
        for (int i = int(scopeMap.size()) - 1; i >= 0; i--)
        {
            if (scopeMap[i].find(name) != scopeMap[i].end())
            {
                return scopeMap[i][name];
            }
        }
        if (globMap.find(name) != globMap.end())
        {
            return globMap[name];
        }
    }
    else
    {
        throw std::runtime_error("Cannot find name " + name);
    }
}

bool TEnv::isVisible(std::string name)
{
    for (int i = int(scopeMap.size()) - 1; i >= 0; i--)
    {
        if (scopeMap[i].find(name) != scopeMap[i].end())
        {
            return true;
        }
    }
    if (globMap.find(name) != globMap.end())
    {
        return true;
    }
    return false;
}

void TEnv::declare(std::string name, Type t)
{
    if (isVisible(name))
    {
        throw std::runtime_error("variable name already used");
    }
    else
    {
        if (scopeMap.size() > 0)
        {
            scopeMap.back()[name] = t;
        }
        else
        {
            globMap[name] = t;
        }
    }
}

void checkProgram(const Program &p)
{
}
