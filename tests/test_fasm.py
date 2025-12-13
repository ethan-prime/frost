from fasm import *

def test_add():
    src = "ADD r0 r1 r2"
    assert asm_add("ADD r0 r1 r2") == 0x1042
    assert asm_add("ADD r7 r5 10") == 0x1F6A

