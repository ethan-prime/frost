import argparse
import sys 

from typing import Optional

def panic(s: str) -> None:
    print(s)
    sys.exit(1)    

def to_int(s: str) -> Optional[int]:
    try:
        return int(s)
    except:
        return None

def reg_to_index(reg: str) -> int:
    reg = reg.lower()
    if len(reg) != 2 or reg[0] != "r":
        panic(f"invalid register {reg}")
    idx = int(reg[1])
    if not (0 <= idx < 8):
        panic(f"invalid register index {idx}")
    return idx

def set_opcode(instr_bits: int, opcode: int) -> int:
   return (opcode << 12) | instr_bits

# insert n starting at ith bit
def set_bit_n(instr_bits: int, i: int, n: int) -> int:
    return instr_bits | (n << i)

def set_bit(instr_bits: int, i: int) -> int:
    return instr_bits | (1 << i)

def asm_add(instr: str) -> int:
    instr = instr.lower().split()
    
    if len(instr) != 4:
        panic("expected 3 parameters with ADD instruction")
    if instr[0] != "add":
        panic("expected ADD instruction")
    dr = instr[1]
    sr1 = instr[2]
    
    instr_bits = set_opcode(0x00, 1)
    instr_bits = set_bit_n(instr_bits, 9, reg_to_index(dr))
    instr_bits = set_bit_n(instr_bits, 6, reg_to_index(sr1))

    if (imm5 := to_int(instr[3])) is not None:
        instr_bits = set_bit_n(set_bit_n(instr_bits, 0, imm5), 5, 1)
    else:
        instr_bits = set_bit_n(instr_bits, 0, reg_to_index(instr[3]))

    return instr_bits

def main():
    parser = argparse.ArgumentParser(
        description="fasm"
    )

    parser.add_argument("input")

    parser.add_argument("--output", type=str, default=None, help="output file")

    args = parser.parse_args()
    
    if (output := args.output) is None:
        output = args.input

    print(args.input)
    print(output)

    print(hex(asm_add("ADD r0 r1 r2")))
    print(hex(asm_add("ADD r7 r5 10")))

if __name__ == "__main__":
    main()

