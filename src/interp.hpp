#pragma once

#include <variant>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include "ast.hpp"
#include <iostream>
#include <optional>

using Value = std::variant<std::string, long long, double, bool, std::monostate>;

std::string valuetoString(const Value &e);

class Env
{
private:
    std::unordered_map<std::string, Value> globMap;
    std::vector<std::vector<std::unordered_map<std::string, Value>>> frameMap;
    std::unordered_map<std::string, const FnDecl *> fnMap;

public:
    void pushFrame();
    void popFrame();
    void pushScope();
    void popScope();
    void declareFn(const FnDecl &fn);
    const FnDecl &lookupFn(const std::string name);
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

class Executor
{
private:
    Env &env;
    Evaluator eval{env};

public:
    Executor(Env &env);

public:
    std::optional<Value> operator()(const Make &e);
    std::optional<Value> operator()(const Assign &e);
    std::optional<Value> operator()(const ExprStmt &e);
    std::optional<Value> operator()(const If &e);
    std::optional<Value> operator()(const While &e);
    std::optional<Value> operator()(const Ret &e);
};

int runProgram(const Program &prog);