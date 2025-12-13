#pragma once
#include <cstdint>
#include "vm.hpp"
#include <iostream>

extern RegFile reg; //register

static std::uint16_t extract(std::uint16_t x, int i, int j) {
    int width = j - i + 1;
    std::uint16_t mask = ((1u << width) - 1u) << i;
    return (x & mask) >> i;
}

static std::uint16_t sign_extend(std::uint16_t x, int bit_count) {
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

void update_flags(Reg r) {
    if (reg[r] == 0) {
        reg[Reg::FLAGS] = static_cast<std::uint32_t>(Flag::Z);
    } else if (reg[r] >> 15) {
        reg[Reg::FLAGS] = static_cast<std::uint32_t>(Flag::N);
    } else {
        reg[Reg::FLAGS] = static_cast<std::uint32_t>(Flag::P);
    }
}

void exec_add(std::uint16_t instr) {
    constexpr std::uint16_t imm_mode = 1u << 5;

    Reg dr = static_cast<Reg>(extract(instr, 9, 11));
    Reg sr1 = static_cast<Reg>(extract(instr, 6, 8));
    
    if (instr & imm_mode) {
        reg[dr] = reg[sr1] + sign_extend(extract(instr, 0, 4), 5);
    } else {
        Reg sr2 = static_cast<Reg>(extract(instr, 0, 2));
        reg[dr] = reg[sr1] + reg[sr2];
    }

    update_flags(dr);
}
