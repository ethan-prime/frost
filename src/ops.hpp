#pragma once
#include <cstdint>
#include "vm.hpp"
#include <iostream>
#include <string>
#include <fmt/core.h>
#include <functional>
#include <cstdlib>

extern RegFile reg; //register

static void panic(const std::string& msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}

static void ill(std::uint32_t instr) {
    panic(fmt::format("illegal instruction 0x{:08X}", instr));
}

static std::uint32_t extract(std::uint32_t x, int i, int j) {
    int width = j - i + 1;
    std::uint32_t mask = ((1u << width) - 1u) << i;
    return (x & mask) >> i;
}

template <int i, int j>
static inline Reg parse_reg(std::uint32_t instr) {
    std::uint8_t reg_idx = extract(instr, i, j);
    return static_cast<Reg>(reg_idx);
}

template <int i, int j>
static inline std::uint16_t parse_i(std::uint32_t instr) {
    return extract(instr, i, j);
}

static inline Reg parse_dr(std::uint32_t instr) {
    return parse_reg<20, 23>(instr);
}

static inline Reg parse_sr1(std::uint32_t instr) {
    return parse_reg<16, 19>(instr);
}

static inline Reg parse_sr2(std::uint32_t instr) {
    return parse_reg<12, 15>(instr);
}

static inline Reg parse_sr(std::uint32_t instr) {
    return parse_reg<16, 19>(instr);
}

static inline Reg parse_base_r(std::uint32_t instr) {
    return parse_reg<16, 19>(instr);
}

static inline std::uint16_t parse_imm16(std::uint32_t instr) {
    return parse_i<0, 15>(instr);
}

static inline std::uint16_t parse_off16(std::uint32_t instr) {
    return parse_i<0, 15>(instr);
}

static inline std::uint16_t parse_trapvec24(std::uint32_t instr) {
    return parse_i<0, 15>(instr);
}

static inline std::uint8_t parse_op(std::uint32_t instr) {
    return parse_i<28, 31>(instr);
}

static inline std::uint8_t parse_subop(std::uint32_t instr) {
    return parse_i<24, 27>(instr);
}

static std::uint32_t sign_extend(std::uint32_t x, int bit_count) {
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFFFFFF << bit_count);
    }
    return x;
}

static void update_flags(Reg r) {
    std::int32_t val = static_cast<std::int32_t>(reg[r]);
    
    if (val < 0) {
        reg[Reg::FLAGS] = static_cast<std::uint32_t>(Flag::N); 
    } else if (val == 0) {
        reg[Reg::FLAGS] = static_cast<std::uint32_t>(Flag::Z);
    } else {
        reg[Reg::FLAGS] = static_cast<std::uint32_t>(Flag::P);
    }
}

template <typename Op>
static void int_alu(Reg dr, Reg sr1, Reg sr2, Op op) {
    reg[dr] = op(reg[sr1], reg[sr2]);
}

static void exec_int_alu(std::uint32_t instr) {
    std::uint8_t subop = parse_subop(instr);
    
    Reg dr = parse_dr(instr);
    Reg sr1 = parse_sr1(instr);
    Reg sr2 = parse_sr2(instr);

    switch (subop) {
        case 0x0: int_alu(dr, sr1, sr2, std::plus<std::uint32_t>{}); break;
        case 0x1: int_alu(dr, sr1, sr2, std::minus<std::uint32_t>{}); break;
        case 0x2: int_alu(dr, sr1, sr2, std::bit_and<std::uint32_t>{}); break;
        case 0x3: int_alu(dr, sr1, sr2, std::bit_or<std::uint32_t>{}); break;
        case 0x4: int_alu(dr, sr1, sr2, std::bit_xor<std::uint32_t>{}); break;
        default: ill(instr); 
    } 

    update_flags(dr);
}

inline void exec_instr(std::uint32_t instr) {
    std::uint8_t opcode = parse_op(instr);

    switch (opcode) {
        case 0x1: exec_int_alu(instr); break;
        default: ill(instr);
    }
}
