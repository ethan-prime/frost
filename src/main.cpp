#include "vm.hpp"
#include "ops.hpp"
#include "debug.hpp"
#include "disassembler.hpp"
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

RegFile reg{};
std::array<std::uint8_t, MEMORY_MAX> memory{};

#define R_PC reg[Reg::PC]
#define R_FLAGS reg[Reg::FLAGS]
#define R_SP reg[Reg::SP]

static std::vector<std::uint8_t> read_all_bytes(std::istream& in) {
    in >> std::noskipws;
    return { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
}

static void load_into_memory(const std::vector<std::uint8_t>& prog, std::uint32_t start_addr) {
    if (start_addr + prog.size() > MEMORY_MAX) throw std::runtime_error("program too large");
    std::fill(memory.begin(), memory.end(), 0);
    std::copy(prog.begin(), prog.end(), memory.begin() + start_addr);
}

static Instr fetch_instr() {
	Instr instr(load<std::uint32_t>(R_PC));
    R_PC += 4;
    return instr;
}



bool IS_HALTED = false;

int main(int argc, const char* argv[]) {
    // parse args

    auto prog = read_all_bytes(std::cin);

    R_FLAGS = static_cast<std::uint32_t>(Flag::Z);
    R_PC = 0x3000;

    load_into_memory(prog, R_PC);

    bool debug = true;
    
	size_t cycle = 0;

    while (!IS_HALTED) {
        Instr instr = fetch_instr();
        exec_instr(instr);
        if (debug) {
			std::println("");
			debug::print_line();
			std::print(" CYCLE= {}\n INSTR= ", cycle); 
            disassembler::print_asm_str(instr);
			debug::print_registers();
			debug::print_mem(reg[Reg::SP], 16);
        }
		cycle++;
    }
}
