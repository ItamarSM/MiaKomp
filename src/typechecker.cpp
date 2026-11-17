#include "typechecker.hpp"

std::string typeToString(Type type)
{
    switch (type)
    {
    case Type::BOOL:
        return "bool";
    case Type::FLOAT:
        return "float";
    case Type::INT:
        return "int";
    case Type::STRING:
        return "string";
    case Type::VOID:
        return "void";
    }
    throw std::runtime_error("type not defined");
}

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
    throw std::runtime_error("Cannot find name " + name);
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

void TEnv::declareFn(std::string name, const FnDecl &fn)
{
    if (fnMap.find(name) == fnMap.end())
    {
        fnMap[name] = &fn;
    }
    else
    {
        throw std::runtime_error("function name alreadu used");
    }
}

const FnDecl &TEnv::lookupFn(std::string name)
{
    if (fnMap.find(name) != fnMap.end())
    {
        return *fnMap[name];
    }
    else
    {
        throw std::runtime_error("no sich function");
    }
}

ExprChecker::ExprChecker(TEnv &tenv) : tenv(tenv) {}

Type ExprChecker::operator()(const IntLit &)
{
    return Type::INT;
}
Type ExprChecker::operator()(const FloatLit &)
{
    return Type::FLOAT;
}
Type ExprChecker::operator()(const BoolLit &)
{
    return Type::BOOL;
}
Type ExprChecker::operator()(const StringLit &)
{
    return Type::STRING;
}
Type ExprChecker::operator()(const Var &e)
{
    return tenv.lookup(e.name);
}
Type ExprChecker::operator()(const Unary &e)
{
    if (e.op == UnaryOp::NEG)
    {
        Type t = std::visit(*this, e.expr->kind);
        if (isNumeric(t))
        {
            return t;
        }
        else
        {
            throw std::runtime_error("cannot negate this type");
        }
    }
    else
    {
        if (std::visit(*this, e.expr->kind) == Type::BOOL)
        {
            return Type::BOOL;
        }
        else
        {
            throw std::runtime_error("cannot negate this type");
        }
    }
}
Type ExprChecker::operator()(const Binary &e)
{
    if (e.op == BinaryOp::AND || e.op == BinaryOp::OR)
    {
        if (std::visit(*this, e.left->kind) == Type::BOOL && std::visit(*this, e.right->kind) == Type::BOOL)
        {
            return Type::BOOL;
        }
        else
        {
            throw std::runtime_error("both operands must be of type bool");
        }
    }
    Type lt = std::visit(*this, e.left->kind);
    Type rt = std::visit(*this, e.right->kind);
    switch (e.op)
    {
    case BinaryOp::ADD:
        if (isNumeric(lt) && isNumeric(rt))
        {
            if (lt == Type::FLOAT || rt == Type::FLOAT)
            {
                return Type::FLOAT;
            }
            else
            {
                return Type::INT;
            }
        }
        else if (lt == Type::STRING && rt == Type::STRING)
        {
            return Type::STRING;
        }
        else
        {
            throw std::runtime_error("Cannot add these types");
        }
    case BinaryOp::SUB:
        if (isNumeric(lt) && isNumeric(rt))
        {
            if (lt == Type::FLOAT || rt == Type::FLOAT)
            {
                return Type::FLOAT;
            }
            else
            {
                return Type::INT;
            }
        }
        else
        {
            throw std::runtime_error("Cannot substract these types");
        }
    case BinaryOp::MUL:
        if (isNumeric(lt) && isNumeric(rt))
        {
            if (lt == Type::FLOAT || rt == Type::FLOAT)
            {
                return Type::FLOAT;
            }
            else
            {
                return Type::INT;
            }
        }
        else
        {
            throw std::runtime_error("Cannot multiply these types");
        }
    case BinaryOp::DIV:
        if (isNumeric(lt) && isNumeric(rt))
        {
            if (lt == Type::INT && rt == Type::INT)
            {
                return Type::INT;
            }
            else
            {
                return Type::FLOAT;
            }
        }
        else
        {
            throw std::runtime_error("Cannot divide these types");
        }
    }
    throw std::runtime_error("cant use the operator on the operands");
}
Type ExprChecker::operator()(const Compare &e)
{
    Type lt = std::visit(*this, e.operands[0]->kind);
    Type rt = std::visit(*this, e.operands[1]->kind);
    if ((e.ops[0] == CompareOp::EQ || e.ops[0] == CompareOp::NOTEQ) && (lt != Type::VOID || rt != Type::VOID))
    {
        if (lt != rt)
        {
            throw std::runtime_error("cannot compare these two operand types");
        }
    }
    else
    {
        if (!(lt == Type::INT && rt == Type::INT || lt == Type::FLOAT && rt == Type::FLOAT))
        {
            throw std::runtime_error("cannot compare these two operand types");
        }
    }
    for (int i = 1; i < int(e.ops.size()); i++)
    {
        lt = rt;
        rt = std::visit(*this, e.operands[i + 1]->kind);
        if ((e.ops[i] == CompareOp::EQ || e.ops[i] == CompareOp::NOTEQ) && (lt != Type::VOID || rt != Type::VOID))
        {
            if (lt != rt)
            {
                throw std::runtime_error("cannot compare these two operand types");
            }
        }
        else
        {
            if (!(lt == Type::INT && rt == Type::INT || lt == Type::FLOAT && rt == Type::FLOAT))
            {
                throw std::runtime_error("cannot compare these two operand types");
            }
        }
    }
    return Type::BOOL;
}

Type ExprChecker::operator()(const Call &e)
{
    if (e.name == "print")
    {
        for (int i = 0; i < int(e.args.size()); i++)
        {
            if (std::visit(*this, e.args[i]->kind) == Type::VOID)
            {
                throw std::runtime_error(std::string("cannot print a void type argument, arg: ") + std::to_string(i));
            }
        }
        return Type::VOID;
    }
    const FnDecl &fn = tenv.lookupFn(e.name);
    int nodefault = 0;
    for (const Param &p : fn.params)
    {
        if (p.value == nullptr)
        {
            nodefault++;
        }
    }
    if (int(e.args.size()) < nodefault || e.args.size() > fn.params.size())
    {
        throw std::runtime_error("wrong number of arguments");
    }
    for (int i = 0; i < int(e.args.size()); i++)
    {
        if (!assignable(fn.params[i].type, std::visit(*this, e.args[i]->kind)))
        {
            throw std::runtime_error("wrong argument types were given");
        }
    }
    return fn.retType;
}

bool isNumeric(Type t)
{
    if (t == Type::FLOAT || t == Type::INT)
    {
        return true;
    }
    return false;
}

bool StmtChecker::operator()(const Make &e)
{
    Type t = std::visit(ep, e.value->kind);
    if (assignable(e.type, t))
    {
        tenv.declare(e.name, e.type);
    }
    else
    {
        throw std::runtime_error("cannot assign " + typeToString(t) + " to " + typeToString(e.type));
    }
    return false;
}

StmtChecker::StmtChecker(TEnv &tenv, ExprChecker &ep, Type retType) : tenv(tenv), ep(ep), retType(retType) {}

bool StmtChecker::operator()(const Assign &e)
{
    Type target = tenv.lookup(e.name);
    Type t = std::visit(ep, e.value->kind);
    if (assignable(target, t))
    {
        return false;
    }
    throw std::runtime_error("cannot assign " + typeToString(t) + " to " + typeToString(target));
}

bool StmtChecker::operator()(const ExprStmt &e)
{
    std::visit(ep, e.expr->kind);
    return false;
}

bool StmtChecker::operator()(const If &e)
{
    Type condt = std::visit(ep, e.condition->kind);
    if (condt != Type::BOOL)
    {
        throw std::runtime_error("condition type must be of type bool");
    }
    bool bodyret = retbody(e.body);
    bool elseret = retbody(e.elseBody);
    return bodyret && elseret;
}

bool StmtChecker::operator()(const While &e)
{
    Type condt = std::visit(ep, e.condition->kind);
    if (condt != Type::BOOL)
    {
        throw std::runtime_error("condition type must be of type bool");
    }
    retbody(e.body);
    return false;
}

bool StmtChecker::operator()(const Ret &e)
{
    Type t = std::visit(ep, e.value->kind);
    if (!assignable(retType, t))
    {
        throw std::runtime_error("wrong return type, expected " + typeToString(retType));
    }
    return true;
}

bool assignable(Type target, Type value)
{
    if (target == value)
    {
        return true;
    }
    else if (target == Type::FLOAT && value == Type::INT)
    {
        return true;
    }
    return false;
}

bool StmtChecker::retbody(const std::vector<std::unique_ptr<Stmt>> &body)
{
    tenv.pushScope();
    bool any = false;
    for (int i = 0; i < int(body.size()); i++)
    {
        if (std::visit(*this, body[i]->stmt) == true)
        {
            any = true;
        }
    }
    tenv.popScope();
    return any;
}

void checkProgram(const Program &p)
{
    TEnv tenv;
    ExprChecker ec{tenv};

    for (const FnDecl &fn : p.funcs)
    {
        tenv.declareFn(fn.name, fn);
    }
    for (const Make &m : p.globals)
    {
        StmtChecker{tenv, ec, Type::VOID}(m);
    }
    for (const FnDecl &fn : p.funcs)
    {
        tenv.pushScope();
        for (const Param &prm : fn.params)
        {
            if (prm.value != nullptr)
            {
                Type t = std::visit(ec, prm.value->kind);
                if (!assignable(prm.type, t))
                {
                    throw std::runtime_error("default of " + prm.name + " has the wrong type");
                }
            }
            tenv.declare(prm.name, prm.type);
        }
        bool returns = StmtChecker{tenv, ec, fn.retType}.retbody(fn.body);
        if (!returns)
        {
            throw std::runtime_error("function " + fn.name + " cannot end without a RET");
        }
        tenv.popScope();
    }
    tenv.pushScope();
    if (!StmtChecker{tenv, ec, Type::INT}.retbody(p.mainFunc))
    {
        throw std::runtime_error("main must RET an int");
    }
    tenv.popScope();
}
