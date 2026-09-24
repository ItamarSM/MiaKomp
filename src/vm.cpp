#include "vm.hpp"

void VM::run(const std::vector<Instruction> &program)
{
    while (this->pc < int(program.size()))
    {
        Instruction inst = program[this->pc];
        switch (inst.opcode)
        {
        case PUSH:
            this->Push(inst.operand);
            this->pc++;
            break;
        case MUL:
            this->Mul();
            this->pc++;
            break;
        case ADD:
            this->Add();
            this->pc++;
            break;
        case SUB:
            this->Sub();
            this->pc++;
            break;
        case DIV:
            this->Div();
            this->pc++;
            break;
        case PRINT:
            std::cout << this->pop() << "\n";
            this->pc++;
            break;
        case STORE:
            this->varMap[int(inst.operand)] = this->pop();
            this->pc++;
            break;
        case LOAD:
            if (this->varMap.find(int(inst.operand)) != this->varMap.end())
            {
                this->stack.push_back(this->varMap[int(inst.operand)]);
            }
            else
            {
                throw std::runtime_error("Invalid var Name (line " + std::to_string(this->pc + 1) + ")\n");
            }
            this->pc++;
            break;
        case JUMP:
            if (!(inst.operand >= 0) || inst.operand > program.size())
            {
                throw std::runtime_error("Invalid line of code: " + std::to_string(inst.operand) + "\n");
            }
            else
            {
                this->pc = int(inst.operand);
            }
            break;
        case JUMP_IF_FALSE:
            if (!(inst.operand >= 0) || inst.operand > program.size())
            {
                throw std::runtime_error("Invalid line of code: " + std::to_string(inst.operand) + "\n");
            }
            else
            {
                if (this->pop() == 0)
                {
                    this->pc = int(inst.operand);
                }
                else
                {
                    this->pc++;
                }
            }
            break;
        case EQ:
            this->Eq();
            this->pc++;
            break;
        case LT:
            this->Lt();
            this->pc++;
            break;
        case GT:
            this->Gt();
            this->pc++;
            break;
        case CALL:
            if (!(inst.operand >= 0) || (inst.operand) >= program.size())
            {
                throw std::runtime_error("Invalid line of code: " + std::to_string(inst.operand) + "\n");
            }
            this->callMap.push_back(this->pc + 1);
            pc = int(inst.operand);
            break;
        case RET:
            pc = this->popAddress();
            break;
        }
    }
}

bool VM::callIsEmpty() const
{
    return this->callMap.empty();
}

void VM::Push(double operand)
{
    this->stack.push_back(operand);
}

void VM::Mul()
{
    double num1 = this->pop();
    double num2 = this->pop();
    this->stack.push_back(num1 * num2);
}

void VM::Add()
{
    double num1 = this->pop();
    double num2 = this->pop();
    this->stack.push_back(num1 + num2);
}

void VM::Sub()
{
    double num2 = this->pop();
    double num1 = this->pop();
    this->stack.push_back(num1 - num2);
}

void VM::Div()
{
    double num2 = this->pop();
    double num1 = this->pop();
    this->stack.push_back(num1 / num2);
}

void VM::Eq()
{
    double num2 = this->pop();
    double num1 = this->pop();
    this->stack.push_back(int(num1 == num2));
}

void VM::Gt()
{
    double num2 = this->pop();
    double num1 = this->pop();
    this->stack.push_back(int(num1 > num2));
}

void VM::Lt()
{
    double num2 = this->pop();
    double num1 = this->pop();
    this->stack.push_back(int(num1 < num2));
}

double VM::pop()
{
    if (this->stack.empty())
    {
        throw std::runtime_error("Stack underflow");
    }
    else
    {
        auto last = this->stack.back();
        this->stack.pop_back();
        return last;
    }
}

int VM::popAddress()
{
    if (this->callMap.empty())
    {
        throw std::runtime_error("Address stack underflow");
    }
    else
    {
        auto last = this->callMap.back();
        this->callMap.pop_back();
        return last;
    }
}