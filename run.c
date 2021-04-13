/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) 
{ 
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

uint32_t ZeroExtImm(short imm) {
    return 0x0000ffff & imm;
}

uint32_t SignExtImm(short imm) {
    return (imm >> 15) ? imm : ZeroExtImm(imm);
}

uint32_t BranchAddr(short imm) {
    return SignExtImm(imm) << 2;
}

uint32_t JumpAddr(int address) {
    return ( (CURRENT_STATE.PC+4) & 0xf0000000 ) | (address << 2);
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
    if(CURRENT_STATE.PC >= MEM_REGIONS[0].start + NUM_INST*4) {
        RUN_BIT = FALSE;
        return;
    }
    // printf("%x %x\n", CURRENT_STATE.PC, MEM_REGIONS[0].start + NUM_INST*4);

    instruction instr = *get_inst_info(CURRENT_STATE.PC);
    unsigned char rs = instr.r_t.r_i.rs;
    unsigned char rt = instr.r_t.r_i.rt;
    short imm = instr.r_t.r_i.r_i.imm;
    unsigned char rd = instr.r_t.r_i.r_i.r.rd;
    unsigned char shamt = instr.r_t.r_i.r_i.r.shamt;
    uint32_t target = instr.r_t.target;

    char incrementPC = 1;
    
    // printf("%d\n", instr.opcode);

    switch(instr.opcode) {
	    //I format
	    case 0x9:		//ADDIU
            CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + SignExtImm(imm);
            break;
	    case 0xc:		//ANDI
            CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & ZeroExtImm(imm);
            break;
	    case 0xf:		//LUI	
            CURRENT_STATE.REGS[rt] = (imm << 16) & 0xffff0000;
            break;
	    case 0xd:		//ORI
            CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | ZeroExtImm(imm);
            break;
	    case 0xb:		//SLTIU
            CURRENT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] < SignExtImm(imm)) ? 1 : 0;
            break;
	    case 0x23:		//LW	
            CURRENT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + SignExtImm(imm));
            break;
	    case 0x2b:		//SW
            mem_write_32(CURRENT_STATE.REGS[rs] + SignExtImm(imm), CURRENT_STATE.REGS[rt]);
            break;
	    case 0x4:		//BEQ
            if(CURRENT_STATE.REGS[rs]==CURRENT_STATE.REGS[rt]) {
                CURRENT_STATE.PC = CURRENT_STATE.PC + 4 + BranchAddr(imm);
                incrementPC = 0;
            }
            break;
	    case 0x5:		//BNE
            if(CURRENT_STATE.REGS[rs]!=CURRENT_STATE.REGS[rt]) {
                CURRENT_STATE.PC = CURRENT_STATE.PC + 4 + BranchAddr(imm);
                incrementPC = 0;
            }
		    break;

    	    //R format
	    case 0x0:		//ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU if JR
            switch(instr.func_code) {
                case 0x21:  //ADDU
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    break;
                case 0x24: //AND
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                    break;
                case 0x08: //JR
                    CURRENT_STATE.PC = CURRENT_STATE.REGS[rs];
                    incrementPC = 0;
                    break;
                case 0x27: //NOR
                    CURRENT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt]);
                    break;
                case 0x25: //OR
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt];
                    break;
                case 0x2b: //SLTU
                    CURRENT_STATE.REGS[rd] = ( CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt] ) ? 1 : 0;
                    break;
                case 0x00: //SLL
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
                    break;
                case 0x02: //SRL
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
                    break;
                case 0x23: //SUBU
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    break;
            }
		    break;

    	    //J format
	    case 0x2:		//J
            CURRENT_STATE.PC = JumpAddr(target);
            incrementPC = 0;
            break;
	    case 0x3:		//JAL
            CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            CURRENT_STATE.PC = JumpAddr(target);
            incrementPC = 0;
            break;

	    default:
            printf("Not available instruction\n");
            assert(0);
	}

    if(incrementPC) CURRENT_STATE.PC += 4;
}
