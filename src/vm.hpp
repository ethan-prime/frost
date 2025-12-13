#pragma once

#include <cstdint>
#include <array>
#include <iostream>
#include <iomanip>

constexpr std::size_t MEMORY_MAX = 1ul << 16;
static std::array<std::uint32_t, MEMORY_MAX> memory{};

enum class Reg : std::uint8_t {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    
    PC, // program counter
    SP, // stack pointer
    FP, // frame pointer
    FLAGS,

    COUNT
};

struct RegFile {
    static constexpr std::size_t count = static_cast<std::size_t>(Reg::COUNT);
    std::array<std::uint32_t, count> registers{};

    std::uint32_t& operator[](Reg r) {
        return registers[static_cast<std::size_t>(r)];
    }

    const std::uint32_t& operator[](Reg r) const {
        return registers[static_cast<std::size_t>(r)];
    }
};

extern RegFile reg;

void print_registers() {
    std::cout << "\n |  ";
    for (int i{}; i < 8; ++i)
        std::cout << "R" << i << "  |  ";
    std::cout << "\n | ";
    for (int i{}; i < 8; ++i) {
        std::cout << std::hex
          << std::setw(4) << std::setfill('0') << reg.registers[i]
          << std::dec << " | ";    
    }
}

enum class Opcode : std::uint8_t {
    BR,
    ADD,
    LD,
    ST,
    JSR, // jump register
    AND,
    LDR, // load register
    STR, // store register
    NOP, // no-op
    NOT,
    LDI, // load indirect
    STI, // store indirect
    JMP,
    RES, // unused
    LEA,
    TRAP,
};

enum class Flag : std::uint32_t {
    P = 1u << 0, // positive
    Z = 1u << 1, // zero
    N = 1u << 2, // negative
};

