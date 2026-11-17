#pragma once

#include "ast.hpp"
#include <unordered_map>
#include <variant>
#include <stdexcept>

std::string typeToString(Type type);
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

class ExprChecker
{
public:
    ExprChecker(TEnv &tenv);

private:
    TEnv &tenv;

public:
    Type operator()(const IntLit &);
    Type operator()(const FloatLit &);
    Type operator()(const BoolLit &);
    Type operator()(const StringLit &);
    Type operator()(const Var &e);
    Type operator()(const Unary &e);
    Type operator()(const Binary &e);
    Type operator()(const Compare &e);
    Type operator()(const Call &e);
};
bool isNumeric(Type t);

class StmtChecker
{
public:
    StmtChecker(TEnv &tenv, ExprChecker &ep, Type retType);

private:
    TEnv &tenv;
    ExprChecker &ep;
    Type retType;

public:
    bool operator()(const Make &e);
    bool operator()(const Assign &e);
    bool operator()(const ExprStmt &e);
    bool operator()(const If &e);
    bool operator()(const While &e);
    bool operator()(const Ret &e);

    bool retbody(const std::vector<std::unique_ptr<Stmt>> &body);
};
bool assignable(Type target, Type value);

void checkProgram(const Program &p);