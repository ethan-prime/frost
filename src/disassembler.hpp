#pragma once
#include "vm.hpp"
#include "ops.hpp"
#include "debug.hpp"

#include <unordered_map>
#include <print>

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

    std::print("{}{}{} ", debug::ansi::fg_red, r_tbl[static_cast<std::uint8_t>(r)], debug::ansi::reset);
}

static void print_imm16(std::uint16_t imm16) {
	std::print("{}0x{:04X} ({}) {} ", debug::ansi::fg_cyan, imm16, static_cast<std::int16_t>(imm16), debug::ansi::reset);
}

static void print_imm12(std::uint16_t imm12) {
	std::print("{}0x{:03X} ({}) {} ", debug::ansi::fg_cyan, imm12, static_cast<std::int16_t>(imm12), debug::ansi::reset);
}

namespace disassembler {
inline void print_asm_str(Instr instr) {
    Op opcode = instr.parse_op();
    Subop subop  = instr.parse_subop();
    
    std::unordered_map<std::uint32_t, std::string> i_tbl = {
        {0x00, "NOP"},
        {0x01, "HALT"},
		{0x03, "CALL"},
		{0x04, "RET"},

        {0x10, "ADD"},
        {0x11, "SUB"},
        {0x12, "AND"},
        {0x13, "OR"},
        {0x14, "XOR"},

		{0x20, "ADDI"},
		{0x21, "ANDI"},
		{0x22, "ORI"},
		{0x23, "XORI"},
		{0x24, "SUBI"},

		{0x31, "LDB"},
		{0x32, "LDBZ"},
		{0x33, "LDW"},
		{0x34, "LDWZ"},
		{0x35, "STB"},
		{0x36, "STW"},
		{0x37, "LEA"},

		{0x40, "JMP"},
		{0x41, "JE"},
		{0x42, "JNE"},
		{0x43, "JL"},
		{0x44, "JG"},
		{0x45, "JGE"},
		{0x46, "JLE"},

		{0x50, "PUSH"},
		{0x51, "POP"},
    }; 


    std::uint8_t code = (opcode << 4) | subop;

    if (i_tbl.count(code)) {
        std::print("{}{}{} ", debug::ansi::fg_lblue, i_tbl[code], debug::ansi::reset);
    } else {
        std::println("instruction 0x{:08X}", static_cast<std::uint32_t>(instr));
    }

    switch(opcode) {
    case 0x0: {
		if (subop == 0x3) {
			print_register(instr.parse_base_r());
			print_imm16(instr.parse_off16());
		};
		break;
	}
    case 0x1: 
        print_register(instr.parse_dr());
        print_register(instr.parse_sr1());
        print_register(instr.parse_sr2());
        break;
	case 0x2:
		print_register(instr.parse_dr());
		print_register(instr.parse_sr());
		print_imm16(instr.parse_imm16());
		break;
	case 0x3:
		print_register(instr.parse_dr());
		print_register(instr.parse_base_r());
		if (subop == 0x7) {
			print_register(instr.parse_sr2());
			print_imm12(instr.parse_off12());
		}
		else
			print_imm16(instr.parse_off16());
		break;
	case 0x4:
		print_register(instr.parse_base_r());
		print_imm16(instr.parse_off16());
		break;
	case 0x5:
		print_register(instr.parse_dr());
		break;
    }

    std::println("(0x{:08X})", static_cast<std::uint32_t>(instr)); 
}
}
