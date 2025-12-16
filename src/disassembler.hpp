#pragma once

#include "vm.hpp"
#include "ops.hpp"

#include <unordered_map>

static void print_register(Reg r) {
    std::unordered_map<std::uint8_t, std::string> r_tbl = {
        {0, "r0"},
        {1, "r1"},
        {2, "r2"},
        {3, "r3"},
        {4, "r4"},
        {5, "r5"},
        {6, "r6"},
        {7, "r7"},
        {8, "r8"},
        {9, "r9"},
        {10, "r10"},
        {11, "pc"},
        {12, "fp"},
        {13, "sp"},
        {14, "ra"},
        {15, "fl"},
    };
    std::cout << r_tbl[static_cast<std::uint8_t>(r)] << " ";
}

namespace disassembler {
inline void print_asm_str(std::uint32_t instr) {
    std::uint8_t opcode = parse_op(instr);
    std::uint8_t subop  = parse_subop(instr);
    
    std::unordered_map<std::uint32_t, std::string> i_tbl = {
        {0x00, "NOP"},
        {0x01, "HALT"},

        {0x10, "ADD"},
        {0x11, "SUB"},
        {0x12, "AND"},
        {0x13, "OR"},
        {0x14, "XOR"},
    }; 


    std::uint8_t code = (opcode << 4) | subop;

    if (i_tbl.count(code)) {
        std::cout << i_tbl[code] << " ";
    } else {
        std::cerr << fmt::format("instruction 0x{:08X}\n", instr);
    }

    switch(opcode) {
    case 0x0: break;
    case 0x1: 
        print_register(parse_dr(instr));
        print_register(parse_sr1(instr));
        print_register(parse_sr2(instr));
        break;
    }

    std::cout << fmt::format("(0x{:08X})", instr) << std::endl; 
}
}
