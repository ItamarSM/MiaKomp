#include <iostream>
#include "vm.hpp"

int main()
{
    std::vector<Instruction> program;

    // 0
    Instruction inst(JUMP, 4);
    program.push_back(inst);

    // 1
    inst = {PUSH, 2};
    program.push_back(inst);

    // 2
    inst = {MUL};
    program.push_back(inst);

    // 3
    inst = {RET};
    program.push_back(inst);

    // 4
    inst = {PUSH, 5};
    program.push_back(inst);

    // 5
    inst = {CALL, 1};
    program.push_back(inst);

    // 6
    inst = {PRINT};
    program.push_back(inst);

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