#pragma once

#include <variant>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <stdexcept>

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