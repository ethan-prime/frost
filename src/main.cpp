#include "vm.hpp"
#include "ops.hpp"
#include "disassembler.hpp"
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

RegFile reg{};
std::array<std::uint8_t, MEMORY_MAX> memory{};

#define R_PC reg[Reg::PC]
#define R_FLAGS reg[Reg::FLAGS]

static std::vector<std::uint8_t> read_all_bytes(std::istream& in) {
    in >> std::noskipws;
    return { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
}

static void load_into_memory(const std::vector<std::uint8_t>& prog, std::uint32_t start_addr) {
    if (start_addr + prog.size() > MEMORY_MAX) throw std::runtime_error("program too large");
    std::fill(memory.begin(), memory.end(), 0);
    std::copy(prog.begin(), prog.end(), memory.begin() + start_addr);
}

static inline std::uint32_t fetch_instr() {
    std::uint32_t instr = load<std::uint32_t>(R_PC);
    R_PC += 4;
    return instr;
}

void print_registers() {
    for (int i=0; i < 10; i++) {
        std::cout << fmt::format("     R{}     |", i);
    }

    
    std::cout << fmt::format("     R10    |");

    std::cout << "\n";

    for(int i=0; i <= 10; i++) {
        std::cout << fmt::format(" 0x{:08X} |", reg.registers[i]);
    }
    

    std::cout << "\n";

    std::cout << fmt::format("     PC     |");
    std::cout << fmt::format("     FP     |");
    std::cout << fmt::format("     SP     |");
    std::cout << fmt::format("     RA     |");
    std::cout << fmt::format("     FL     |");
    
    std::cout << "\n";

    for(int i=11; i < 16; i++) {
        std::cout << fmt::format(" 0x{:08X} |", reg.registers[i]);
    }
    std::cout << "\n";
}

bool IS_HALTED = false;

int main(int argc, const char* argv[]) {
    // parse args

    auto prog = read_all_bytes(std::cin);

    R_FLAGS = static_cast<std::uint32_t>(Flag::Z);
    R_PC = 0x3000;

    load_into_memory(prog, R_PC);

    bool debug = true;
    
    while (!IS_HALTED) {
        std::uint32_t instr = fetch_instr();
        exec_instr(instr);
        if (debug) {
            disassembler::print_asm_str(instr); 
            print_registers();
        }
    }
}
