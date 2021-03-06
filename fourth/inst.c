/*
 * Added more commands and addressing mode tests.
 * This is reflected in memconfig.c as various code blocks.
 * Considerable time was spent testing each addressing mode, as once
 * proven working, it will work for all commands.
 */

#include "fsm.h"

/*
 * nbits = number of bits to shift
 * number = number being worked on
 * Set a Bit: 	number |= 1 << nbits;
 * Clear a Bit:	number &= ~(1 << nbits);
 * Flip a Bit:	number ^= 1 << nbits;
 * Check a Bit:	result = (number >> nbits) & 1; 
 */


int I_ADC(uchar c){
	printf("ADC \t");
	switch (c){
		case 0x69: printf("Immediate\tOP A   \tL 2\tT 2\n"); break;
		case 0x65: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x75: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0x6d: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0x7d: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		case 0x79: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		case 0x61: printf("x,indirect\tOP ($nn,x)\tL 2\tT 6\n"); break;
		case 0x71: printf("Y,indrect\tOP ($nn),y\tL 2\tT 5\n"); break;
		default: printf("!!!!!\n");
	}
	return 0;
} // End of ADC

int I_AND(uchar c){  // Untested
	switch (c){
		case 0x29:	// Immediate
					CPU_A&=AM_IMMEDIATE();
					break;
		case 0x25:	// Zero Page
					CPU_A&=AM_ZERO_PAGE();
					break;
		case 0x35:	// Zero Page,X
					CPU_A&=AM_ZP_X();
					break;
		case 0x2d:	// Absolute
					CPU_A&=AM_ABSOLUTE();
					break;
		case 0x3d:	// Absolute,X
					CPU_A&=AM_ABS_X();
					break;
		case 0x39:	// Absolute,Y
					CPU_A&=AM_ABS_Y();
					break;
		case 0x21:	// Indirect,X
					CPU_A&=AM_IND_X();
					break;
		case 0x31:	// Indirect,Y
					CPU_A&=AM_IND_Y();
					break;
		default: break;
	}
	if ((CPU_A >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Sign bit
	if (CPU_A==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if A=0 else clear
	return 0;
}

int I_ASL(uchar c){
	printf("ASL\t");

	switch (c){
		case 0x0a: printf("Accumulator\tASL A   \tL 1\tT 2\n"); break;
		case 0x06: printf("Zero Page\tASL $nn  \tL 2\tT 5\n"); break;
		case 0x16: printf("Zero Page,X\tASL $nn,x\tL 2\tT 6\n"); break;
		case 0x0e: printf("Absolute\tASL $nnnn\tL 3\tT 6\n"); break;
		case 0x1e: printf("Absolute,X\tASL $nnnn,x\tL 3\tT 7\n"); break;
		default: printf("ASL  !!!!!\n");
	}
	return 0;
}  // End of I_ASL

int I_BCH(uchar c){
	switch (c){
		case 0x10: printf("BPL\tBranch if Plus\n"); break;
		case 0x30: printf("BMI\tBranch if Minus\n"); break;
		case 0x50: printf("BVC\tBranch if Overflow Clear\n"); break;
		case 0x70: printf("BVS\tBranch if Overflow Set\n"); break;
		case 0x90: printf("BCC\tBranch if Carry Clear\n"); break;
		case 0xb0: printf("BCS\tBranch if Carry Set\n"); break;
		case 0xd0: printf("BNE\tBranch if Not Equal\n"); break;
		case 0xf0: printf("BEQ\tBranch if Equal\n"); break;
		default: printf("!!!!!\n");
	}
	return 0;
}

int I_BIT(uchar c){
	printf("BIT\t");
	switch (c){
		case 0x24: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x2c: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}
	
	return 0;
} // End BIT

int I_BRK(uchar c){  // Completed
	int j;
	memory[0x100+(CPU_SP--)]=(CPU_PC>>8);
	memory[0x100+(CPU_SP--)]=(CPU_PC);
	memory[0x100+(CPU_SP--)]=(CPU_SR^16); // Set Break bit on SR pushed to stack as per documentation
	CPU_PC=memory[0xfffe];
	CPU_PC<<=8;
	CPU_PC|=memory[0xffff];
	CPU_PC=(CPU_PC>>8)|(CPU_PC<<8);  // Swap bytes
	return 0;
}

int I_CMP(uchar c){
	printf("CMP\t");
	switch (c){
		case 0xc9: printf("Immediate\tOP A    \tL 2\tT 2\n"); break;
		case 0xc5: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xd5: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0xcd: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0xdd: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		case 0xd9: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		case 0xc1: printf("x,indirect\tOP ($nn,x)\tL 2\tT 6\n"); break;
		case 0xd1: printf("Y,indrect\tOP ($nn),y\tL 2\tT 5\n"); break;
		default: printf("!!!!!\n");
	}
	return 0;
}  // End CMP

int I_CPX(uchar c){	
	printf("CPX\t");
	switch (c){
		case 0xe0: printf("Immediate\tOP A    \tL 2\tT 2\n"); break;
		case 0xe4: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xec: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}	
	return 0;
}  // End CPX

int I_CPY(uchar c){
	printf("CPY\t");
	switch (c){
		case 0xc0: printf("Immediate\tOP A    \tL 2\tT 2\n"); break;
		case 0xc4: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xcc: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}		
	return 0;
}  // End CPY

int I_DEC(uchar c){
	printf("DEC\t");
	switch (c){
		case 0xc6: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xd6: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0xce: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0xde: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}
	return 0;
} // End DEC

int I_EOR(uchar c){  // Untested
	printf("EOR\t");
	switch (c){
		case 0x49: // Immediate
					CPU_A^=AM_IMMEDIATE();
					break;
		case 0x45: // Zero Page
					CPU_A^=AM_ZERO_PAGE();
					break;
		case 0x55: // Zero Page,X
					CPU_A^=AM_ZP_X();
					break;
		case 0x4d: // Absolute
					CPU_A^=AM_ABSOLUTE();
					break;
		case 0x5d: // Absolute,X
					CPU_A^=AM_ABS_X();
					break;
		case 0x59: // Absolute,Y
					CPU_A^=AM_ABS_Y();
					break;
		case 0x41: // Indirect,X
					CPU_A^=AM_IND_X();
					break;
		case 0x51: // Indirect,Y
					CPU_A^=AM_IND_Y();
					break;
		default: break;
	}
	return 0;
}  // End EOR

int I_FLG(uchar c){	
	switch (c){
		case 0x18: CPU_SR &=~(1); break;  // CLC
		case 0x38: CPU_SR |=1; break;  // SEC
		case 0x58: CPU_SR &=~(4); break;  // CLI
		case 0x78: CPU_SR |=4; break;  // SEI
		case 0xb8: CPU_SR &=~64; break;  // CLV
		case 0xd8: CPU_SR &=~8; break;  // CLD
		case 0xf8: CPU_SR |=8; break;  // SED
		default: printf("!!!!!\n");
	}
	return 0;
}

int I_INC(uchar c){
	printf("INC\t");
	switch (c){
		case 0xe6: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xf6: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0xee: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0xfe: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}	
	return 0;
}  // End INC

int I_JMP(uchar c){
	unsigned short int Temp;
	switch (c){
		case 0x4c: // Absolute	
					Temp=memory[CPU_PC++];
					Temp|=(memory[CPU_PC++])<<8;
					CPU_PC=Temp;
					break;
		case 0x6c: // Only the Jump uses indirect, so put code here
					Temp=memory[CPU_PC++];
					Temp|=(memory[CPU_PC++])<<8;
					CPU_PC=Temp;
					Temp=memory[CPU_PC++];
					Temp|=(memory[CPU_PC++])<<8;
					CPU_PC=Temp;
					break;
		default: break;
	}
	return 0;
}  // End JMP

int I_JSR(uchar c){  // Completed
	unsigned short int Temp_PC;
	Temp_PC=memory[CPU_PC++];
	Temp_PC=memory[CPU_PC]<<8;
	memory[0x100+(CPU_SP--)]=(CPU_PC)>>8;
	memory[0x100+(CPU_SP--)]=(CPU_PC);
	CPU_PC=Temp_PC;	
	return 0;
}  // End JSR

int I_LDA(uchar c){  // Completed
	switch (c){
		case 0xA9:  // Immediate
					CPU_A=AM_IMMEDIATE();
					break;
		case 0xA5: // Zero Page
					CPU_A=AM_ZERO_PAGE();
					break;
		case 0xB5: // Zero Page,X
					CPU_A=AM_ZP_X();
					break;
		case 0xAd:  // Absolute
					CPU_A=AM_ABSOLUTE();
					break;
		case 0xbd:  // Absolute,X
					CPU_A=AM_ABS_X();
					break;
		case 0xb9:  // Absolute,Y
					CPU_A=AM_ABS_Y();
					break;
		case 0xa1:  // Indirext,X
					CPU_A=AM_IND_X();
					break;
		case 0xb1: // Indirext,Y
					CPU_A=AM_IND_Y();
					break;
		default: printf("!!!!!\n");
	}	
	return 0;
}  // End LDA

int I_LDX(uchar c){  // Completed
	switch (c){
		case 0xa2: // Immediate
					CPU_X=AM_IMMEDIATE();
					break;
		case 0xa6: // Zero Page
					CPU_X=AM_ZERO_PAGE();
					break;
		case 0xb6: // Zero Page,Y
					CPU_X=AM_ZP_Y();
					break;
		case 0xae: // Absolute
					CPU_X=AM_ABSOLUTE();
					break;
		case 0xbe:  // Absolute,Y
					CPU_X=AM_ABS_Y();
					break;
		default: break;
	}
	return 0;
}  // End LDX

int I_LDY(uchar c){  // Completed
	switch (c){
		case 0xa0: // Immediate
					CPU_Y=AM_IMMEDIATE();
					break;
		case 0xa4: // Zero Page
					CPU_Y=AM_ZERO_PAGE();
					break;
		case 0xb4: // Zero Page,X
					CPU_Y=AM_ZP_X();
					break;
		case 0xac: // Absolute
					CPU_Y=AM_ABSOLUTE();
					break;
		case 0xbc: // Absolute,X
					CPU_Y=AM_ABS_X();
					break;
		default: printf("!!!!!\n");
	}	
	return 0;
}  // End LDY

int I_LSR(uchar c){
	printf("LSR\t");
	switch (c){
		case 0x4A: printf("Accumulator\tOP A   \tL 1\tT 2\n"); break;
		case 0x46: printf("Zero Page\tOP $nn  \tL 2\tT 5\n"); break;
		case 0x56: printf("Zero Page,X\tOP $nn,x\tL 2\tT 6\n"); break;
		case 0x4e: printf("Absolute\tOP $nnnn\tL 3\tT 6\n"); break;
		case 0x5e: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 7\n"); break;
		default: printf("!!!!!\n");
	}		
	return 0;
}  // End LSR

int I_NOP(uchar c){	return 0;} // Do Nothing

int I_ORA(uchar c){  // Untested
	uchar Temp;
	switch (c){
		case 0x09: // Immediate ORA #$nn
					CPU_A|=AM_IMMEDIATE();
					break;
		case 0x05: // Zero Page
					CPU_A|=AM_ZERO_PAGE();
					break;
		case 0x15: // Zero Page,X
					CPU_A|=AM_ZP_X();
					break;
		case 0x0d: // Absolute
					CPU_A|=AM_ABSOLUTE();
					break;
		case 0x1d: // Absolute,X
					CPU_A|=AM_ABS_X();
					break;
		case 0x19: // Absolute,Y
					CPU_A&=AM_ABS_Y();
					break;
		case 0x01: // Indirect,X
					CPU_A|=AM_IND_X();
					break;
		case 0x11: // Indirect,Y
					CPU_A|=AM_IND_Y();
					break;
		default: printf("!!!!!\n");
	}
	if ((CPU_A >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Sign bit
	if (CPU_A==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if A=0 else clear

	return 0;
}  // End or I_ORA

int I_REG(uchar c){  // Completed
	// All Implied instructions of 1 byte, 2 Cycles
	char flag=0;

	switch (c){
		case 0xAA: CPU_X=CPU_A; flag=1; break; // TAX
		case 0x8A: CPU_A=CPU_X; break;  // TXA
		case 0xCA: CPU_X-=1; flag=1;  break;  // DEX
		case 0xE8: CPU_X+=1; flag=1; break;  // INX
		case 0xA8: CPU_Y=CPU_A; flag=2; break;  //TAY
		case 0x98: CPU_A=CPU_Y; break;  // TYA
		case 0x88: CPU_Y-=1; flag=2; break; // DEY
		case 0xC8: CPU_Y+=1; flag=2; break; // INY
		default: break;
	}
	switch (flag){  // Set SR depending on which Register was target
		case 0: break; // Working with A Register
				if ((CPU_A >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Sign bit
				if (CPU_A==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if A=0 else clear
				break;
		case 1:	// Working with X Register
				if ((CPU_X >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Sign bit
				if (CPU_X==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if X=0 else clear
				break;
		case 2: // Working with Y Register
				if ((CPU_Y >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Sign bit
				if (CPU_Y==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if Y=0 else clear
				break;
		default:break;  // Never hit, as there are only 3 registers; and flag is initialised to zero
	} 
	return 0;
}  // End REG

int I_ROL(uchar c){
	printf("ROL\t");
	switch (c){
		case 0x1a: printf("Accumulator\tOP A   \tL 2\tT 2\n"); break;
		case 0x26: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x36: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0x2e: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0x3e: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}		
	return 0;
}  // End ROL

int I_ROR(uchar c){
	printf("ROR\t");
	switch (c){
		case 0x6a: printf("Accumulator\tOP A   \tL 2\tT 2\n"); break;
		case 0x66: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x76: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0x6e: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0x7e: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}		
	return 0;
}  // End ROR

int I_RTI(uchar c){  // Completed
/*
 * Remove Status Byte and PC from stack in that order
 * The return address is the actual address NOT address+1 as in RTS
 * */	
	CPU_SR=memory[0x100+(++CPU_SP)];
	CPU_PC=(memory[0x100+(++CPU_SP)]);
	CPU_PC|=memory[(0x100+(++CPU_SP))]<<8;

	return 0;
}  // End RTI

int I_RTS(uchar c){  // Completed
/*
 * Compensate for JSR not putting actual return address onto stack
 */
	if(CPU_SP!=0xff) {  // Return from Subroutine
		CPU_PC=memory[0x100+(++CPU_SP)]+1;
		CPU_PC|=memory[(0x100+(++CPU_SP))]<<8;
	} else { DONE=1; // Return to Monitor
	} 
	return 0;
}  // End RTS

int I_SBC(uchar c){
	printf("SBC\t");
	switch (c){
		case 0xE9: printf("Immediate\tOP A   \tL 2\tT 2\n"); break;
		case 0xE5: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xF5: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0xed: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0xfd: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		case 0xf9: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		case 0xe1: printf("x,indirect\tOP ($nn,x)\tL 2\tT 6\n"); break;
		case 0xf1: printf("Y,indrect\tOP ($nn),y\tL 2\tT 5\n"); break;
		default: printf("!!!!!\n");
	}		
	return 0;
}  // End SBC

int I_STA(uchar c){
	printf("STA\t");
	switch (c){
		case 0x85: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x95: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0x8d: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0x9d: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		case 0x99: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		case 0x81: printf("x,indirect\tOP ($nn,x)\tL 2\tT 6\n"); break;
		case 0x91: printf("Y,indrect\tOP ($nn),y\tL 2\tT 5\n"); break;
		default: printf("!!!!!\n");
	}		
	return 0;
}  // End STA

int I_STC(uchar c){  // Completed
	switch (c){
		case 0x9a: CPU_SP=CPU_X; break;  // TXS: no Flags affected
		case 0xba: CPU_X=CPU_SP; break;  // TSX: no Flags affected
		case 0x48: memory[CPU_SP--]=CPU_A; break;  // PHA: no Flags affected
		case 0x68: CPU_A=memory[++CPU_SP]; break;  // PLA: no Flags affected
		case 0x08: memory[CPU_SP--]=CPU_SR; break; // PHP: no Flags affected
		case 0x28: CPU_SR=memory[++CPU_SP]; break; // PLP: no Flags affected
		default: break;
	}
	return 0;
}  // End STC

int I_STX(uchar c){
	printf("STX\t");
	switch (c){
		case 0x86: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x96: printf("Zero Page,Y\tOP $nn,y\tL 2\tT 4\n"); break;
		case 0x8e: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}		
	return 0;
}  // End STX

int I_STY(uchar c){
	printf("STY\t");
	switch (c){
		case 0x84: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x94: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0x8c: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}			
	return 0;
}  // End STY



// These are the Illegal or undefined instructions
int I_UDEF(uchar c){
	printf("...\n");
	
	return 0;
}  // End of I_UDEF
