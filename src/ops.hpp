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

using Op = std::uint8_t;
using Subop = std::uint8_t;


struct Instr {
	std::uint32_t instr;
	Instr(std::uint32_t instr_) : instr(instr_) {};

	inline operator std::uint32_t() const noexcept {
		return instr;
	}

	std::uint32_t extract(int i, int j) {
		int width = j - i + 1;
		std::uint32_t mask = ((1u << width) - 1u) << i;
		return (instr & mask) >> i;	
	}

	inline Reg parse_reg(int i, int j) {
		std::uint8_t reg_idx = extract(i, j);
		return static_cast<Reg>(reg_idx);
	}

	inline std::uint16_t parse_i(int i, int j) {
		return extract(i, j);
	}

	inline Reg parse_dr() {
		return parse_reg(20, 23);
	}

	inline Reg parse_sr1() {
		return parse_reg(16, 19);
	}

	inline Reg parse_sr2() {
		return parse_reg(12, 15);
	}

	inline Reg parse_sr() {
		return parse_reg(16, 19);
	}

	inline Reg parse_base_r() {
		return parse_reg(16, 19);
	}

	inline std::uint16_t parse_imm16() {
		return parse_i(0, 15);
	}

	inline std::uint16_t parse_off16() {
		return parse_i(0, 15);
	}

	inline std::uint16_t parse_off12() {
		return parse_i(0, 11);
	}

	inline std::uint16_t parse_trapvec24() {
		return parse_i(0, 15);
	}

	Op parse_op() {
		return parse_i(28, 31);
	}

	Subop parse_subop() {
		return parse_i(24, 27);
	}
};

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

static void exec_int_alu(Instr instr) {
    Subop subop = instr.parse_subop();
    
    Reg dr = instr.parse_dr();
    Reg sr1 = instr.parse_sr1();
    Reg sr2 = instr.parse_sr2();

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

static void exec_int_imm(Instr instr) {
	Subop subop = instr.parse_subop();

	Reg dr = instr.parse_dr();
	Reg sr = instr.parse_sr1();
	std::uint32_t imm16 = sign_extend(instr.parse_imm16(), 16);

	switch (subop) {
		case 0x0: int_imm(dr, sr, imm16, std::plus<std::uint32_t>{}); break;
		case 0x1: int_imm(dr, sr, imm16, std::bit_and<std::uint32_t>{}); break; 
		case 0x2: int_imm(dr, sr, imm16, std::bit_or<std::uint32_t>{}); break; 
		case 0x3: int_imm(dr, sr, imm16, std::bit_xor<std::uint32_t>{}); break; 
		case 0x4: int_imm(dr, sr, imm16, std::minus<std::uint32_t>{}); break; 
		default: ill(instr);
	}

	update_flags(dr);
}

static void exec_call(Instr instr) {
	Reg base_r = instr.parse_base_r();
	std::uint32_t off16 = sign_extend(instr.parse_off16(), 16);

	reg[Reg::SP] -= 4;
	store<std::uint32_t>(reg[Reg::SP], reg[Reg::PC]);

	reg[Reg::PC] = reg[base_r] + off16;
}

static void exec_ret() {
	reg[Reg::PC] = load<std::uint32_t>(reg[Reg::SP]);
	reg[Reg::SP] += 4;
}

static void exec_special(Instr instr) {
    Subop subop = instr.parse_subop();

    switch (subop) {
        // case 0x0: return; // no-op
        case 0x1: IS_HALTED = true; return;
        
		case 0x3: exec_call(instr); return;
		case 0x4: exec_ret(); return;
		default: ill(instr);
    }
}

static void exec_mem(Instr instr) {
	Subop subop = instr.parse_subop();

	Reg base_r = instr.parse_base_r();
	Reg dr = instr.parse_dr();
	std::uint32_t off16 = instr.parse_off16();

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
		case 0x7:
			reg[dr] = reg[instr.parse_sr1()] + reg[instr.parse_sr2()] * instr.parse_off12();
			break;
		default: ill(instr);
	}
}

static void exec_stack(Instr instr) {
	Subop subop = instr.parse_subop();

	Reg dr = instr.parse_dr();

	switch (subop) {
	case 0x0:
		reg[Reg::SP] -= 4;
		store<std::uint32_t>(reg[Reg::SP], reg[dr]);
		break;
	case 0x1: 
		reg[dr] = load<std::uint32_t>(reg[Reg::SP]);
		reg[Reg::SP] += 4;
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

static void exec_jump(Instr instr) {
	Subop subop = instr.parse_subop();

	Reg base_r = instr.parse_base_r();
	std::uint32_t off16 = sign_extend(instr.parse_off16(), 16);

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

inline void exec_instr(Instr instr) {
    std::uint8_t opcode = instr.parse_op();

    switch (opcode) {
        case 0x0: exec_special(instr); break;
        case 0x1: exec_int_alu(instr); break;
		case 0x2: exec_int_imm(instr); break; 
		case 0x3: exec_mem(instr); break;
		case 0x4: exec_jump(instr); break;
		case 0x5: exec_stack(instr); break; 
		default: ill(instr);
    }
}
