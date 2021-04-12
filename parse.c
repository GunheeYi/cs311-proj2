/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

char* substr(char* str, int s, int e) {
	char* new = (char*) malloc(sizeof(char)*(e-s+2));
	strncpy(new, str+s, e-s+1);
	new[e-s+1] = 0;
	return new;
}

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;
	/** Implement this function */
	instr.opcode = fromBinary(substr(buffer, 0, 5));

	printf("%s ", buffer);

	switch(instr.opcode) {
	    //I format
	    case 0x9:		//ADDIU
	    case 0xc:		//ANDI
	    case 0xf:		//LUI	
	    case 0xd:		//ORI
	    case 0xb:		//SLTIU
	    case 0x23:		//LW	
	    case 0x2b:		//SW
	    case 0x4:		//BEQ
	    case 0x5:		//BNE
			instr.r_t.r_i.rs = fromBinary(substr(buffer, 6, 10));
			instr.r_t.r_i.rt = fromBinary(substr(buffer, 11, 15));
			instr.r_t.r_i.r_i.imm = fromBinary(substr(buffer, 16, 31));
			// printf("%c %d %d %d %d\n", 'I', instr.opcode, instr.r_t.r_i.rs, instr.r_t.r_i.rt, instr.r_t.r_i.r_i.imm);
			break;

		//R format
	    case 0x0:		//ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU if JR
			instr.r_t.r_i.rs = fromBinary(substr(buffer, 6, 10));
			instr.r_t.r_i.rt = fromBinary(substr(buffer, 11, 15));
			instr.r_t.r_i.r_i.r.rd = fromBinary(substr(buffer, 16, 20));
			instr.r_t.r_i.r_i.r.shamt = fromBinary(substr(buffer, 21, 25));
			instr.func_code = fromBinary(substr(buffer, 26, 31));
			// printf("%c %d %d %d %d %d %d\n", 'R', instr.opcode, instr.r_t.r_i.rs, instr.r_t.r_i.rt, instr.r_t.r_i.r_i.r.rd, instr.r_t.r_i.r_i.r.shamt, instr.func_code);
			break;

		//J format
	    case 0x2:		//J
	    case 0x3:		//JAL
			instr.r_t.target = fromBinary(substr(buffer, 6, 31));
			// printf("%c %d %d\n", 'J', instr.opcode, instr.r_t.target);
			break;

	    // default:
		// printf("Not available instruction\n");
		// assert(0);
	}


    return instr;
}

void parsing_data(const char *buffer, const int index)
{
	/** Implement this function */
	mem_write_32(MEM_REGIONS[0].size + index, fromBinary(buffer));
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
	printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
	printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	switch(INST_INFO[i].opcode)
	{
	    //I format
	    case 0x9:		//ADDIU
	    case 0xc:		//ANDI
	    case 0xf:		//LUI	
	    case 0xd:		//ORI
	    case 0xb:		//SLTIU
	    case 0x23:		//LW	
	    case 0x2b:		//SW
	    case 0x4:		//BEQ
	    case 0x5:		//BNE
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
		break;

    	    //R format
	    case 0x0:		//ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU if JR
		printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
		printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
		break;

    	    //J format
	    case 0x2:		//J
	    case 0x3:		//JAL
		printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
		break;

	    default:
		printf("Not available instruction\n");
		assert(0);
	}
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
	printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
	printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
