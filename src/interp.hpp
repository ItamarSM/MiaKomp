#pragma once

#include <variant>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include "ast.hpp"

using Value = std::variant<std::string, long long, double, bool, std::monostate>;

std::string valuetoString(const Value &e);

class Env
{
private:
    std::unordered_map<std::string, Value> globMap;
    std::vector<std::unordered_map<std::string, Value>> scopeMap;

public:
    void pushScope();
    void popScope();
    void declare(std::string name, const Value &e);
    Value &lookup(std::string name);
    void assign(std::string name, const Value &e);
};

bool checkKey(const std::vector<std::unordered_map<std::string, Value>> &e, std::string name);

class Evaluator
{
private:
    Env &env;

public:
    Evaluator(Env &env);

public:
    Value operator()(const IntLit &e);
    Value operator()(const FloatLit &e);
    Value operator()(const StringLit &e);
    Value operator()(const BoolLit &e);
    Value operator()(const Var &e);
    Value operator()(const Unary &e);
    Value operator()(const Binary &e);
    Value operator()(const Compare &e);
    Value operator()(const Call &e);
};