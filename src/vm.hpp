#pragma once

#include <cstdint>
#include <array>

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

