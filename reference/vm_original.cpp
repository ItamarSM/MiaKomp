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
    void run(const std::vector<Instruction> &program)
    {
        while (pc < int(program.size()))
        {
            Instruction inst = program[pc];
            switch (inst.opcode)
            {
            case PUSH:
                Push(inst.operand);
                pc++;
                break;
            case MUL:
                Mul();
                pc++;
                break;
            case ADD:
                Add();
                pc++;
                break;
            case SUB:
                Sub();
                pc++;
                break;
            case DIV:
                Div();
                pc++;
                break;
            case PRINT:
                std::cout << pop() << "\n";
                pc++;
                break;
            case STORE:
                varMap[int(inst.operand)] = pop();
                pc++;
                break;
            case LOAD:
                if (varMap.find(int(inst.operand)) != varMap.end())
                {
                    stack.push_back(varMap[int(inst.operand)]);
                }
                else
                {
                    throw std::runtime_error("Invalid var Name (line " + std::to_string(pc + 1) + ")\n");
                }
                pc++;
                break;
            case JUMP:
                if (!(inst.operand >= 0) || inst.operand > program.size())
                {
                    throw std::runtime_error("Invalid line of code: " + std::to_string(inst.operand) + "\n");
                }
                else
                {
                    pc = int(inst.operand);
                }
                break;
            case JUMP_IF_FALSE:
                if (!(inst.operand >= 0) || inst.operand > program.size())
                {
                    throw std::runtime_error("Invalid line of code: " + std::to_string(inst.operand) + "\n");
                }
                else
                {
                    if (pop() == 0)
                    {
                        pc = int(inst.operand);
                    }
                    else
                    {
                        pc++;
                    }
                }
                break;
            case EQ:
                Eq();
                pc++;
                break;
            case LT:
                Lt();
                pc++;
                break;
            case GT:
                Gt();
                pc++;
                break;
            case CALL:
                if (!(inst.operand >= 0) || (inst.operand) >= program.size())
                {
                    throw std::runtime_error("Invalid line of code: " + std::to_string(inst.operand) + "\n");
                }
                callMap.push_back(pc + 1);
                pc = int(inst.operand);
                break;
            case RET:
                pc = popAddress();
                break;
            }
        }
    }
    [[nodiscard]] bool callIsEmpty() const
    {
        return callMap.empty();
    }

private:
    void Push(double operand)
    {
        stack.push_back(operand);
    }
    void Mul()
    {
        double num1 = pop();
        double num2 = pop();
        stack.push_back(num1 * num2);
    }
    void Add()
    {
        double num1 = pop();
        double num2 = pop();
        stack.push_back(num1 + num2);
    }
    void Sub()
    {
        double num2 = pop();
        double num1 = pop();
        stack.push_back(num1 - num2);
    }
    void Div()
    {
        double num2 = pop();
        double num1 = pop();
        stack.push_back(num1 / num2);
    }
    void Eq()
    {
        double num2 = pop();
        double num1 = pop();
        stack.push_back(int(num1 == num2));
    }
    void Gt()
    {
        double num2 = pop();
        double num1 = pop();
        stack.push_back(int(num1 > num2));
    }
    void Lt()
    {
        double num2 = pop();
        double num1 = pop();
        stack.push_back(int(num1 < num2));
    }
    double pop()
    {
        if (stack.empty())
        {
            throw std::runtime_error("Stack underflow");
        }
        else
        {
            auto last = stack.back();
            stack.pop_back();
            return last;
        }
    }
    int popAddress()
    {
        if (callMap.empty())
        {
            throw std::runtime_error("Address stack underflow");
        }
        else
        {
            auto last = callMap.back();
            callMap.pop_back();
            return last;
        }
    }

private:
    std::unordered_map<int, double> varMap{};
    std::vector<int> callMap{};
};

int main()
{
    std::vector<Instruction> program;

    // 0
    Instruction inst(PUSH, 5);
    program.push_back(inst);

    // 1 subRoutine B
    inst = {CALL, 3};
    program.push_back(inst);

    // 2
    inst = {PRINT};
    program.push_back(inst);

    // 3
    inst = {PUSH, 2};
    program.push_back(inst);

    // 4 subRoutine A
    inst = {MUL};
    program.push_back(inst);

    // 5
    inst = {PRINT};
    program.push_back(inst);

    // 6

    // 7

    // 8

    // 9

    // 10

    // 10

    // 11

    // 12
    // 13

    // 14

    // 15

    // 16

    // 17

    VM virtualMachine{};
    try
    {
        virtualMachine.run(program);
        if (!virtualMachine.stack.empty())
        {
            throw std::runtime_error("Stack isnt empty");
        }
        if (!virtualMachine.callIsEmpty())
        {
            throw std::runtime_error("Call stack isnt empty");
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}