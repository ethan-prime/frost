#include "vm.hpp"
#include "ops.hpp"
#include <iostream>

RegFile reg{};

int main(int argc, const char* argv[]) {
    // parse args

    reg[Reg::FLAGS] = static_cast<std::uint32_t>(Flag::Z);
    reg[Reg::PC] = 0x3000;

    bool running = true;
    bool debug = true;
    
    memory[reg[Reg::PC]] = 0x00001021;

    while (running) {
        // fetch
        // top 16 bits unused
        uint16_t instr = memory[reg[Reg::PC]] & 0xFFFF;
        uint8_t op = instr >> 12;

        switch (static_cast<Opcode>(op))
        {
            case Opcode::ADD:
                exec_add(instr);
                break;
            case Opcode::AND:
                break;
            case Opcode::NOT:
                break;
            case Opcode::BR:
                break;
            case Opcode::JMP:
                break;
            case Opcode::JSR:
                break;
            case Opcode::LD:
                break;
            case Opcode::LDI:
                break;
            case Opcode::LDR:
                break;
            case Opcode::LEA:
                break;
            case Opcode::ST:
                break;
            case Opcode::STI:
                break;
            case Opcode::STR:
                break;
            case Opcode::TRAP:
                break;
            case Opcode::RES:
            case Opcode::NOP:
            default:
                break;
        }
        if (debug) print_registers();
        reg[Reg::PC]++;
    }
}
