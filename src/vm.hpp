#pragma once

#include <cstdint>
#include <array>
#include <fmt/core.h>

constexpr std::size_t MEMORY_MAX = 1ul << 16;
extern std::array<std::uint8_t, MEMORY_MAX> memory;

void panic(const std::string&);

template <typename T>
static inline T load(std::uint32_t addr) {
    if (addr + sizeof(T) > MEMORY_MAX) panic(fmt::format("out of bounds memory access at 0x{:08X}", addr));
    
    T v = 0;
    for (std::size_t i = 0; i < sizeof(T); i++) {
        v |= static_cast<T>(memory[addr + i]) << (8 * i);
    }

    return v;
}

template <typename T>
static inline void store(std::uint32_t addr, T v) {
    if (addr + sizeof(T) > MEMORY_MAX) panic(fmt::format("out of bounds memory access at 0x{:08X}", addr));

    for (std::size_t i = 0; i < sizeof(T); i++) {
        memory[addr + i] = static_cast<std::uint8_t>(v >> (8 * i));
    }
}

enum class Reg : std::uint8_t {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,

    PC,
    FP,
    SP,
    RA,
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

enum class Flag : std::uint32_t {
    P = 1 << 0,
    N = 1 << 1,
    Z = 1 << 2,
};

