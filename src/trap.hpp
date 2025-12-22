#include <print>
#include <cstdlib>
#include "vm.hpp"

namespace trap {

void putc() {
	std::putchar(reg[Reg::R1]);
}

void null_trap_excp(std::uint16_t trapvec) {
	std::println("invalid trapvec 0x{:04X}", trapvec);
}

void exec_trapvec(std::uint16_t trapvec) {
	switch (trapvec) {
		case 0x1: putc(); break;
		default: null_trap_excp(trapvec);
	}	
}
}
