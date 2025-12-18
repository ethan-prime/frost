# frost
A virtual machine written in C++.

fasm.py: assembler script (frost asm -> binary)  
fvm: virtual machine which executes binary instructions

```bash
usage: python3 fasm.py <file.fasm> | ./build/fvm
```

## frost asm specification
| Category                    | Mnemonic        | Opcode | Sub-Op / Flag | Operands              |
| --------------------------- | --------------- | ------ | ------------- | --------------------- |
| **Special**                 | NOP             | `0x0`  | `0x0`         | —                     |
|                             | HALT            | `0x0`  | `0x1`         | —                     |
|                             | TRAP            | `0x0`  | `0x2`         | `TRAPVEC24`           |
|                             | CALL            | `0x0`  | `0x3`         | `BASE_R, OFF16`       |
|                             | RET             | `0x0`  | `0x4`         | —                     |
| **Integer ALU (RRR)**       | ADD             | `0x1`  | `0x0`         | `DR, SR1, SR2`        |
|                             | SUB             | `0x1`  | `0x1`         | `DR, SR1, SR2`        |
|                             | AND             | `0x1`  | `0x2`         | `DR, SR1, SR2`        |
|                             | OR              | `0x1`  | `0x3`         | `DR, SR1, SR2`        |
|                             | XOR             | `0x1`  | `0x4`         | `DR, SR1, SR2`        |
| **Integer Immediate (RRI)** | ADDI            | `0x2`  | `0x0`         | `DR, SR, IMM16`       |
|                             | ANDI            | `0x2`  | `0x1`         | `DR, SR, IMM16`       |
|                             | ORI             | `0x2`  | `0x2`         | `DR, SR, IMM16`       |
|                             | XORI            | `0x2`  | `0x3`         | `DR, SR, IMM16`       |
|                             | SUBI            | `0x2`  | `0x4`         | `DR, SR, IMM16`       |
| **Memory**                  | LDB             | `0x3`  | `0x1`         | `DR, BASE_R, OFF16`   |
|                             | LDBZ (zero-ext) | `0x3`  | `0x2`         | `DR, BASE_R, OFF16`   |
|                             | LDW             | `0x3`  | `0x3`         | `DR, BASE_R, OFF16`   |
|                             | LDWZ (zero-ext) | `0x3`  | `0x4`         | `DR, BASE_R, OFF16`   |
|                             | STB             | `0x3`  | `0x5`         | `DR, BASE_R, OFF16`   |
|                             | STW             | `0x3`  | `0x6`         | `DR, BASE_R, OFF16`   |
|                             | LEA             | `0x3`  | `0x7`         | `DR, SR1, SR2, OFF12` |
| **Jump**                    | JMP             | `0x4`  | `0x0`         | `BASE_R, OFF16`       |
|                             | JE              | `0x4`  | `0x1`         | `BASE_R, OFF16`       |
|                             | JNE             | `0x4`  | `0x2`         | `BASE_R, OFF16`       |
|                             | JL              | `0x4`  | `0x3`         | `BASE_R, OFF16`       |
|                             | JG              | `0x4`  | `0x4`         | `BASE_R, OFF16`       |
|                             | JGE             | `0x4`  | `0x5`         | `BASE_R, OFF16`       |
|                             | JLE             | `0x4`  | `0x6`         | `BASE_R, OFF16`       |
| **Stack**                   | PUSH            | `0x5`  | `0x0`         | `SR`                  |
|                             | POP             | `0x5`  | `0x1`         | `DR`                  |
