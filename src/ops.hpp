#pragma once
#include <cstdint>
#include "vm.hpp"
#include <iostream>
#include <string>
#include <fmt/core.h>
#include <functional>
#include <cstdlib>

extern RegFile reg; //register
extern bool IS_HALTED;

inline void panic(const std::string& msg) {
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

inline std::uint8_t parse_op(std::uint32_t instr) {
    return parse_i<28, 31>(instr);
}

inline std::uint8_t parse_subop(std::uint32_t instr) {
    return parse_i<24, 27>(instr);
}

inline std::uint32_t sign_extend(std::uint32_t x, int bit_count) {
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

template <typename Op>
static void int_imm(Reg dr, Reg sr, std::uint32_t imm16, Op op) {
	reg[dr] = op(reg[sr], imm16);
}

static void exec_int_imm(std::uint32_t instr) {
	std::uint8_t subop = parse_subop(instr);

	Reg dr = parse_dr(instr);
	Reg sr = parse_sr1(instr);
	std::uint32_t imm16 = sign_extend(parse_imm16(instr), 16);

	switch (subop) {
		case 0x0: int_imm(dr, sr, imm16, std::plus<std::uint32_t>{}); break;
		case 0x1: int_imm(dr, sr, imm16, std::bit_and<std::uint32_t>{}); break; 
		case 0x2: int_imm(dr, sr, imm16, std::bit_or<std::uint32_t>{}); break; 
		case 0x3: int_imm(dr, sr, imm16, std::bit_xor<std::uint32_t>{}); break; 
		default: ill(instr);
	}

	update_flags(dr);
}

static void exec_special(std::uint32_t instr) {
    std::uint8_t subop = parse_subop(instr);

    switch (subop) {
        case 0x0: return; // no-op
        case 0x1: IS_HALTED = true; return;
        default: ill(instr);
    }
}

static void exec_mem(std::uint32_t instr) {
	std::uint8_t subop = parse_subop(instr);

	Reg base_r = parse_base_r(instr);
	Reg dr = parse_dr(instr);
	std::uint32_t off16 = parse_off16(instr);


	switch (subop) {
		case 0x1: 
		case 0x2: {
			std::uint32_t byte = load<std::uint8_t>(reg[base_r] + off16);
			if (subop == 0x1) {
				byte = sign_extend(byte, 8);
			}
			reg[dr] = byte;
			break;
		}
		case 0x3:
			reg[dr] = load<std::uint32_t>(reg[base_r] + off16);
			break;
		case 0x5:
			store<std::uint8_t>(reg[base_r] + off16, reg[dr] & 0xFF);
			break;
		case 0x6:
			store<std::uint32_t>(reg[base_r] + off16, reg[dr]);
			break;
	}
}


template <Flag... Fs>
inline constexpr std::uint32_t flag_mask = 
	(0u | ... | static_cast<std::uint32_t>(Fs));

template <Flag... Fs>
inline bool take_jump() {
	return (reg[Reg::FLAGS] & flag_mask<Fs...>);
}

template <Flag... Fs>
static void jump_flags(Reg base_r, std::uint32_t off16) {
	if (take_jump<Fs...>())
		reg[Reg::PC] = reg[base_r] + off16;
}

static void exec_jump(std::uint32_t instr) {
	std::uint8_t subop = parse_subop(instr);

	Reg base_r = parse_base_r(instr);
	std::uint32_t off16 = sign_extend(parse_off16(instr), 16);

	switch (subop) {
		case 0x0: jump_flags<Flag::Z, Flag::N, Flag::P>(base_r, off16); break;
		case 0x1: jump_flags<Flag::Z>(base_r, off16); break;
		case 0x2: jump_flags<Flag::P, Flag::N>(base_r, off16); break;
		case 0x3: jump_flags<Flag::N>(base_r, off16); break;
		case 0x4: jump_flags<Flag::P>(base_r, off16); break;
		case 0x5: jump_flags<Flag::P, Flag::Z>(base_r, off16); break;
		case 0x6: jump_flags<Flag::N, Flag::Z>(base_r, off16); break;
	}
}

inline void exec_instr(std::uint32_t instr) {
    std::uint8_t opcode = parse_op(instr);

    switch (opcode) {
        case 0x0: exec_special(instr); break;
        case 0x1: exec_int_alu(instr); break;
		case 0x2: exec_int_imm(instr); break; 
		case 0x3: exec_mem(instr); break;
		case 0x4: exec_jump(instr); break;
		default: ill(instr);
    }
}
