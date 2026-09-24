#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <limits>
#include <unordered_map>
#include <string>

enum Op
{
    PUSH,
    MUL,
    ADD,
    SUB,
    DIV,
    PRINT,
    LOAD,
    STORE,
    JUMP,
    JUMP_IF_FALSE,
    EQ,
    LT,
    GT,
    CALL,
    RET
};

struct Instruction
{
public:
    Op opcode;
    double operand;

public:
    Instruction(Op opcode, double operand = std::numeric_limits<double>::quiet_NaN()) : opcode(opcode),
                                                                                        operand(operand) {}
};

class VM
{
public:
    std::vector<double> stack;
    int pc = 0;
    void run(const std::vector<Instruction> &program);
    [[nodiscard]] bool callIsEmpty() const;

private:
    void Push(double operand);
    void Mul();
    void Add();
    void Sub();
    void Div();
    void Eq();
    void Gt();
    void Lt();
    double pop();
    int popAddress();

private:
    std::unordered_map<int, double> varMap{};
    std::vector<int> callMap{};
};