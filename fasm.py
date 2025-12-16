import argparse
import sys 
from enum import Enum

from typing import Optional, List

class Reg(Enum):
    R0 = 0
    R1 = 1
    R2 = 2
    R3 = 3
    R4 = 4
    R5 = 5
    R6 = 6
    R7 = 7
    R8 = 8
    R9 = 9
    R10 = 10
    PC = 11
    FP = 12
    SP = 13
    RA = 14
    FLAGS = 15

def panic(s: str) -> None:
    print(s)
    sys.exit(1)    

def parse_int(s: str) -> Optional[int]:
    try:
        return 0xffff & int(s)
    except:
        panic("expected int")

def parse_reg(reg_str: str) -> Reg:
    reg_str = reg_str.lower()
    reg_list = ["r0", "r1", "r2", "r3", "r4", "r5",
                "r6", "r7", "r8", "r9", "r10",
                "pc", "fp", "sp", "ra"]

    if reg_str not in reg_list:
        panic(f"register {reg_str} invalid")

    return Reg(reg_list.index(reg_str))

class Instr:
    def __init__(self):
        self.instr = 0x0

    def set_opcode(self, opcode: int):
        self.instr |= opcode << 28
        return self

    def set_subop(self, subop: int):
        self.instr |= subop << 24
        return self

    def set_dr(self, dr: Reg):
        self.instr |= dr.value << 20
        return self

    def set_sr1(self, sr1: Reg):

        self.instr |= sr1.value << 16
        return self

    def set_sr2(self, sr2: Reg):
        self.instr |= sr2.value << 12
        return self

    def set_sr(self, sr: Reg):
        self.instr |= sr.value << 16
        return self
    
    def set_base_r(self, base_r: Reg):
        self.instr |= base_r.value << 16
        return self

    def set_imm16(self, imm16: int):
        self.instr |= imm16
        return self

    def set_off16(self, off16: int):

        self.instr |= off16
        return self

    def set_trapvec24(self, trapvec24: int):
        self.instr |= trapvec24
        return self

    def as_int(self) -> int:
        return self.instr

    def parse_instr(self, parse_str: str, instr_str: str):
        func_tbl = {
            "sr" : (parse_reg, self.set_sr),
            "sr1": (parse_reg, self.set_sr1),
            "sr2": (parse_reg, self.set_sr2),
            "dr" : (parse_reg, self.set_dr),
            "base_r" : (parse_reg, self.set_base_r),
            "imm16" : (parse_int, self.set_imm16),
            "off16" : (parse_int, self.set_off16),
            "trapvec24" : (parse_int, self.set_trapvec24)
        }

        parse_ = parse_str.replace(",", "").split()
        instr_ = instr_str.replace(",", "").split()

        match instr_[0].upper():
            case "NOP"  : self.set_opcode(0x0).set_subop(0x0)
            case "HALT" : self.set_opcode(0x0).set_subop(0x1)
            case "TRAP" : self.set_opcode(0x0).set_subop(0x2)
            case "ADD"  : self.set_opcode(0x1).set_subop(0x1)
            case "SUB"  : self.set_opcode(0x1).set_subop(0x1)
            case "AND"  : self.set_opcode(0x1).set_subop(0x2)
            case "OR"   : self.set_opcode(0x1).set_subop(0x3)
            case "XOR"  : self.set_opcode(0x1).set_subop(0x4)
            case "ADDI" : self.set_opcode(0x2).set_subop(0x0)
            case "ANDI" : self.set_opcode(0x2).set_subop(0x1)
            case "ORI"  : self.set_opcode(0x2).set_subop(0x2)
            case "XORI" : self.set_opcode(0x2).set_subop(0x3)
            case "LDB"  : self.set_opcode(0x3).set_subop(0x1)
            case "LDBZ" : self.set_opcode(0x3).set_subop(0x2)
            case "LDW"  : self.set_opcode(0x3).set_subop(0x3)
            case "LDWZ" : self.set_opcode(0x3).set_subop(0x4)
            case "STB"  : self.set_opcode(0x3).set_subop(0x5)
            case "STW"  : self.set_opcode(0x3).set_subop(0x6)
            case "JMP"  : self.set_opcode(0x4).set_subop(0x0)
            case "JE"   : self.set_opcode(0x4).set_subop(0x1)
            case "JNE"  : self.set_opcode(0x4).set_subop(0x2)
            case "JL"   : self.set_opcode(0x4).set_subop(0x3)
            case "JG"   : self.set_opcode(0x4).set_subop(0x4)
            case "JGE"  : self.set_opcode(0x4).set_subop(0x5)
            case "JLE"  : self.set_opcode(0x4).set_subop(0x6)
            case _: panic(f"illegal instruction {instr_str}")
 
        for i in range(1, len(parse_)):
            s = parse_[i]
            if s == ";": break
            if s not in func_tbl:
                panic(f"unrecognized parse_str {s}")
            parse, self.set = func_tbl[s]
            self.set(parse(instr_[i]))

        return self

def print_code(instrs: List[Instr]):
    for idx, i in enumerate(instrs):
        if idx % 6 == 0:
            print("\n")
        else:
            print(", ", end="")

        print(f"{i.as_int() & 0xffffffff:08x}", end="")
    print()

def main():
    parser = argparse.ArgumentParser(
        description="fsm"
    )

    parser.add_argument("input")

    parser.add_argument("--output", type=str, default=None, help="output file")

    args = parser.parse_args()
    
    if (output := args.output) is None:
        output = args.input

    with open(args.input) as f:
        instrs = f.readlines()

        res = []

        for i_str in instrs:
            i_str = i_str.strip()
            if i_str == "": continue
            match i_str.split(" ")[0].upper():
                    case "NOP"  : i = Instr().parse_instr("NOP", i_str)
                    case "HALT" : i = Instr().parse_instr("HALT", i_str)
                    case "TRAP" : i = Instr().parse_instr("TRAP trapvec24", i_str)
                    case "ADD"  : i = Instr().parse_instr("ADD dr sr1 sr2", i_str)
                    case "SUB"  : i = Instr().parse_instr("SUB dr sr1 sr2", i_str)
                    case "AND"  : i = Instr().parse_instr("AND dr sr1 sr2", i_str)
                    case "OR"   : i = Instr().parse_instr("OR dr sr1 sr2", i_str)
                    case "XOR"  : i = Instr().parse_instr("XOR dr sr1 sr2", i_str)
                    case "ADDI" : i = Instr().parse_instr("ADDI dr sr imm16", i_str)
                    case "ANDI" : i = Instr().parse_instr("ANDI dr sr imm16", i_str)
                    case "ORI"  : i = Instr().parse_instr("ORI dr sr imm16", i_str)
                    case "XORI" : i = Instr().parse_instr("XORI dr sr imm16", i_str)
                    case "LDB"  : i = Instr().parse_instr("LDB dr base_r off16", i_str)
                    case "LDBZ" : i = Instr().parse_instr("LDBZ dr base_r off16", i_str)
                    case "LDW"  : i = Instr().parse_instr("LDW dr base_r off16", i_str)
                    case "LDWZ"  : i = Instr().parse_instr("LDWZ dr base_r off16", i_str)
                    case "STB"  : i = Instr().parse_instr("STB dr base_r off16", i_str)
                    case "STW"  : i = Instr().parse_instr("STW dr base_r off16", i_str)
                    case "JMP"  : i = Instr().parse_instr("JMP base_r off16", i_str)
                    case "JE"   : i = Instr().parse_instr("JE base_r off16", i_str)
                    case "JNE"  : i = Instr().parse_instr("JNE base_r off16", i_str)
                    case "JL"   : i = Instr().parse_instr("JL base_r off16", i_str)
                    case "JG"   : i = Instr().parse_instr("JG base_r off16", i_str)
                    case "JGE"  : i = Instr().parse_instr("JGE base_r off16", i_str)
                    case "JLE"  : i = Instr().parse_instr("JLE base_r off16", i_str)
                    case _: panic(f"illegal instruction {i_str}")
            
            res.append(i)

    print_code(res)
    return [i.as_int() for i in res]

if __name__ == "__main__":
    main()

