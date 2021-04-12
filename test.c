#include <stdio.h>
#include <stdint.h>

uint32_t ZeroExtImm(short imm) {
    return 0x0000ffff & imm;
}

uint32_t SignExtImm(short imm) {
    return (imm >> 15) ? imm : ZeroExtImm(imm);
}

uint32_t BranchAddr(short imm) {
    return SignExtImm(imm) << 2;
}

int main() {
    // printf("%8x\n", SignExtImm(0xffff));
    // printf("%8x\n", SignExtImm(0x7fff));
    // printf("%8x\n", ZeroExtImm(0xffff));
    // printf("%8x\n", ZeroExtImm(0x7fff));
    // printf("%8x\n", BranchAddr(0xffff));
    // printf("%8x\n", BranchAddr(0x7fff));

    union {
        int a;
        short b;
        char c;
    } hello;
    hello.a = 0x12345678;
    printf("%8x\n", hello.a);
    printf("%8x\n", hello.b);
    printf("%8x\n", hello.c);
}