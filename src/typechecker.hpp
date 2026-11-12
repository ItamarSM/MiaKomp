#pragma once

#include "ast.hpp"
#include <unordered_map>
#include <variant>
#include <stdexcept>

class TEnv
{
private:
    std::unordered_map<std::string, Type> globMap;
    std::vector<std::unordered_map<std::string, Type>> scopeMap;
    std::unordered_map<std::string, const FnDecl *> fnMap;

public:
    void pushScope();
    void popScope();
    Type lookup(std::string name);
    bool isVisible(std::string name);
    void declare(std::string name, Type t);
    void declareFn(std::string name, const FnDecl &fn);
    const FnDecl &lookupFn(std::string name);
};

void checkProgram(const Program &p);