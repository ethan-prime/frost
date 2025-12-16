#include "vm.hpp"
#include "ops.hpp"

RegFile reg{};

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

int main(int argc, const char* argv[]) {
    // parse args

    reg[Reg::FLAGS] = static_cast<std::uint32_t>(Flag::Z);
    reg[Reg::PC] = 0x3000;

    bool running = true;
    bool debug = true;
    
    memory[reg[Reg::PC]] = 0x14000000;

    while (running) {
        // fetch instr
        std::uint32_t instr = memory[reg[Reg::PC]];
        exec_instr(instr);
        if (debug) print_registers();
        reg[Reg::PC]++;
    }
}
