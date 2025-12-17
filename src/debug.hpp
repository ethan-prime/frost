#pragma once
#include <string_view>
#include <print>
#include "vm.hpp"

extern RegFile reg;

namespace debug {
namespace ansi {
    constexpr std::string_view reset = "\033[0m";
    constexpr std::string_view fg_red   = "\x1b[38;5;203m";
    constexpr std::string_view fg_cyan = "\x1b[38;5;73m";
    constexpr std::string_view fg_lblue = "\x1b[38;5;111m"; 
    constexpr std::string_view fg_dim   = "\x1b[38;5;250m";
	constexpr std::string_view bg_dark  = "\x1b[48;5;235m";
	constexpr std::string_view bg_dim   = "\x1b[48;5;250m";
}

RegFile last_reg{};

void print_line() {
	constexpr int N = 142;
	std::string line;
	line.reserve(N*3);

	constexpr char utf8_double_hline[] = "\xE2\x95\x90";

	for (int i = 0; i < N; i++)
		line += utf8_double_hline;

	std::println("{}{}{}", ansi::fg_dim, line, ansi::reset);
}

void print_registers() {

	print_line();

    for (int i = 0; i < 10; i++) {
        std::print("{}{}     r{}     {}|", ansi::bg_dark, ansi::fg_red, i, ansi::reset);
    }
	
    std::print("{}{}     r10    {}|", ansi::bg_dark, ansi::fg_red, ansi::reset);

    std::println("");

    for (int i = 0; i <= 10; i++) {
		if (last_reg.registers[i] != reg.registers[i])
			std::print("{}{} 0x{:08X} {}|", ansi::bg_dark, ansi::fg_cyan, reg.registers[i], ansi::reset);
		else 
			std::print("{}{} 0x{:08X} {}|", ansi::bg_dark, ansi::fg_dim, reg.registers[i], ansi::reset);
    }
    

	std::println("");

    std::print("{}{}     pc     {}|", ansi::bg_dark, ansi::fg_red, ansi::reset);
    std::print("{}{}     fp     {}|", ansi::bg_dark, ansi::fg_red, ansi::reset);
    std::print("{}{}     sp     {}|", ansi::bg_dark, ansi::fg_red, ansi::reset);
    std::print("{}{}     ra     {}|", ansi::bg_dark, ansi::fg_red, ansi::reset);
    std::print("{}{}     fl     {}|", ansi::bg_dark, ansi::fg_red, ansi::reset);
    
    std::println("");

    for (int i = 11; i < 16; i++) {
		if (last_reg.registers[i] != reg.registers[i])
			std::print("{}{} 0x{:08X} {}|", ansi::fg_cyan, ansi::bg_dark, reg.registers[i], ansi::reset);
		else
			std::print("{}{} 0x{:08X} {}|", ansi::fg_dim, ansi::bg_dark, reg.registers[i], ansi::reset);
    
	}
	std::println("");
	
	print_line();

	last_reg = reg;
}

void _print_mem(std::uint32_t addr, std::size_t n_bytes) {
	std::print("{}{}0x{:08X}{}", ansi::fg_red, ansi::bg_dark, addr, ansi::reset);
	std::print("{}{}= ", ansi::bg_dark, ansi::fg_dim);
	for (std::size_t i = 0; i < n_bytes; ++i) {
		std::print("0x{:02X} ", load<std::uint8_t>(addr+i));
	}
	std::println("{}", ansi::reset);
}

void print_mem(std::uint32_t addr, std::size_t n_bytes) {
	std::size_t printed = 0;
	for (size_t i = 0; printed < n_bytes; ++i) {
		if (n_bytes - printed > 8) {
			_print_mem(addr + 8 * i, 8);
			printed += 8;
		} else {
			_print_mem(addr + 8 * i, n_bytes - printed);
			printed += (n_bytes - printed);
		}
	}
}

}
