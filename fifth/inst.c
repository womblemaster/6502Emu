/*
 * Found a flaw in my way of using the addressing modes.  for any instruction
 * that only reads, the previous method worked fine.  But for any instruction
 * that needed to write to the memory, then the method was unable to do this.
 * For this reason, it was changed so the addressing mode only passes back
 * the index, not the actual value.  This way, we added the extra wrapper
 * round the returned value, as in: memory[ADDRESSING_MODE_CALL()];
 * This way, we can use temporary variables to allow generic code to be used
 * to perform the actual manipulation.
 * 
 * For the Store instructions, we can use the same addressing mode routines,
 * in exactly the same way as indexes. So: memory[ADDRESSING_MODE_CALL()]=Something;
 * 
 * All instructions now have code, most have been tested, but not all.
 *
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


int I_ADC(uchar c){  // in progress
	char A,M,C;  // Temp A, Temp Mem, Carry as Signed bytes
	switch (c){
		case 0x69:	// Immediate
					M=memory[AM_IMMEDIATE()]; // Put Immediate value into Temp
					break;
		case 0x65:	// Zero Page
					M=memory[AM_ZERO_PAGE()]; break;
		case 0x75:  // Zero Page,X
					M=memory[AM_ZP_X()]; break;
		case 0x6d:  // Absolute
					M=memory[AM_ABSOLUTE()]; break;
		case 0x7d:  // Absolute,X
					M=memory[AM_ABS_X()]; break;
		case 0x79:  // Absolute,Y
					M=memory[AM_ABS_Y()]; break;
		case 0x61: 	// Indirect,X
					M=memory[AM_IND_X()]; break;
		case 0x71:  // Indirect,Y
					M=memory[AM_IND_Y()]; break;
		default: printf("!!!!!\n");
	}
	C=CPU_SR&1; // store Current Carry
	// Will it overflow
	if ((CPU_A^(CPU_A+M+C)&(CPU_A^M))) CPU_SR|=64; else CPU_SR&=~64;
	// Carry flag needed?
	CPU_SR|=(CPU_A>(CPU_A+M)); // Set carry if it is needed.
	// Calculate
	CPU_A+=M+C; // Add with Carry
	SET_SR(0);  // Set SR based on CPU_A
	return 0;
} // End of ADC

int I_AND(uchar c){  // Untested
	switch (c){
		case 0x29:	// Immediate
					CPU_A&=memory[AM_IMMEDIATE()];
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
	SET_SR(0); // Set flags based on result
	return 0;
}

int I_ASL(uchar c){ // Completed
/*
 * Left shift all bits by 1, setting Carry to MSB (bit 7)
 * Set LSB (bit 0) to 0
 * 
 * it makes sense here to use pointers, as we need to manipulate
 * more than one type of entity (Accumulator or Memory).
 */
	uchar C,*MPTR;

	switch (c){
		case 0x0a: // Accumulator
					MPTR=&CPU_A; break;
		case 0x06: // Zero Page
					MPTR=&memory[AM_ZERO_PAGE()]; break;
		case 0x16: // Zero Page,X
					MPTR=&memory[AM_ZP_X()]; break;
		case 0x0e: // Absolute
					MPTR=&memory[AM_ABSOLUTE()]; break;
		case 0x1e: // Absolute,X
					MPTR=&memory[AM_ABS_X()]; break;
		default: break;
	}
	CPU_SR|=(*MPTR)>>7; // Set carry from Left most Bit
	(*MPTR)=(*MPTR)<<1; // Left Shift
	if (((*MPTR) >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Negative Sign bit
	if ((*MPTR)==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if A=0 else clear	
	return 0;
}  // End of I_ASL

int I_BCH(uchar c){  // untested
	uchar flag=0; // don't branch by default
	unsigned short offset;
	switch (c){
		case 0x10: (CPU_SR&=0x80)?(flag=0):(flag=1); break; // BPL
		case 0x30: (CPU_SR&=0x80)?(flag=1):(flag=0); break; // BMI
		case 0x50: (CPU_SR&=0x40)?(flag=0):(flag=1); break; // BVC
		case 0x70: (CPU_SR&=0x40)?(flag=1):(flag=0); break; // BVS
		case 0x90: (CPU_SR&=1)?(flag=0):(flag=1); break; // BCC
		case 0xb0: (CPU_SR&=1)?(flag=1):(flag=0); break; // BCS
		case 0xd0: (CPU_SR&=2)?(flag=0):(flag=1); break; // BNE
		case 0xf0: (CPU_SR&=2)?(flag=1):(flag=0); break;  // BEQ
		default: break;
	}
	// Calculate the offset
	offset=(unsigned short)memory[CPU_PC]; // cast byte to unsigned short for negative addition
	if (flag) CPU_PC+=offset; else CPU_PC++;
	
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

int I_CMP(uchar c){  // Complete
	uchar M; // Temp Memory
	switch (c){
		case 0xc9: // Immediate
					M=memory[AM_IMMEDIATE()];
					break;
		case 0xc5: // Zero Page
					M=memory[AM_ZERO_PAGE()];
					break;
		case 0xd5: // Zero Page,X
					M=memory[AM_ZP_X()];
					break;
		case 0xcd: // Absolute
					M=memory[AM_ABSOLUTE()];
					break;
		case 0xdd: // Absolute,X
					M=memory[AM_ABS_X()];
					break;
		case 0xd9: // Absolute,Y
					M=memory[AM_ABS_Y()];
					break;
		case 0xc1: // Indirect,X
					M=memory[AM_IND_X()];
					break;
		case 0xd1: // Indirect,Y
					M=memory[AM_IND_Y()];
					break;
		default: break;
	}
	if (CPU_A>=M) CPU_SR|=1; else CPU_SR&=~1; // Set or Clear Carry
	if (CPU_A==0) CPU_SR|=2; else CPU_SR&=~2; // Set or Clear Zero Flag
	if (CPU_A>0x79) CPU_SR|=128; else CPU_SR&=~128; // Set Clear Sign (Negative) Flag
	return 0;
}  // End CMP

int I_CPX(uchar c){	// Complete
	uchar M; // Temp Memory
	switch (c){
		case 0xe0: // Immediate
					M=memory[AM_IMMEDIATE()];
					break;
		case 0xe4: // Zero Page
					M=memory[AM_ZERO_PAGE()];
					break;
		case 0xec: // Absolute
					M=memory[AM_ABSOLUTE()];
					break;
		default: break;
	}
	if (CPU_X>=M) CPU_SR|=1; else CPU_SR&=~1; // Set or Clear Carry
	if (CPU_X==0) CPU_SR|=2; else CPU_SR&=~2; // Set or Clear Zero Flag
	if (CPU_X>0x79) CPU_SR|=128; else CPU_SR&=~128; // Set Clear Sign (Negative) Flag
	return 0;
}  // End CPX

int I_CPY(uchar c){  // Complete
	uchar M; // Temp Memory
	switch (c){
		case 0xc0: // Immediate
					M=memory[AM_IMMEDIATE()];
					break;
		case 0xc4: // Zero Page
					M=memory[AM_ZERO_PAGE()];
					break;
		case 0xcc: // Absolute
					M=memory[AM_ABSOLUTE()];
					break;
		default: break;
	}		
	if (CPU_Y>=M) CPU_SR|=1; else CPU_SR&=~1; // Set or Clear Carry
	if (CPU_Y==0) CPU_SR|=2; else CPU_SR&=~2; // Set or Clear Zero Flag
	if (CPU_Y>0x79) CPU_SR|=128; else CPU_SR&=~128; // Set Clear Sign (Negative) Flag
	return 0;
}  // End CPY

int I_DEC(uchar c){  // Complete
	uchar *MPTR;
	switch (c){
		case 0xc6: // Zero Page
					MPTR=&memory[AM_ZERO_PAGE()]; break;
		case 0xd6: // Zero Page,X
					MPTR=&memory[AM_ZP_X()]; break;
		case 0xce: // Absolute
					MPTR=&memory[AM_ABSOLUTE()]; break;
		case 0xde: // Absolute,X
					MPTR=&memory[AM_ABS_X()]; break;
		default: break;
	}
	(*MPTR)--;
	if (((*MPTR) >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Negative Sign bit
	if ((*MPTR)==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if A=0 else clear

	return 0;
} // End DEC

int I_EOR(uchar c){  // Untested
	printf("EOR\t");
	switch (c){
		case 0x49: // Immediate
					CPU_A^=memory[AM_IMMEDIATE()];
					break;
		case 0x45: // Zero Page
					CPU_A^=memory[AM_ZERO_PAGE()];
					break;
		case 0x55: // Zero Page,X
					CPU_A^=memory[AM_ZP_X()];
					break;
		case 0x4d: // Absolute
					CPU_A^=memory[AM_ABSOLUTE()];
					break;
		case 0x5d: // Absolute,X
					CPU_A^=memory[AM_ABS_X()];
					break;
		case 0x59: // Absolute,Y
					CPU_A^=memory[AM_ABS_Y()];
					break;
		case 0x41: // Indirect,X
					CPU_A^=memory[AM_IND_X()];
					break;
		case 0x51: // Indirect,Y
					CPU_A^=memory[AM_IND_Y()];
					break;
		default: break;
	}
	return 0;
}  // End EOR

int I_FLG(uchar c){	 // Completed
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

int I_INC(uchar c){  // Completed untested
	// Let's use pointers again
	uchar *MPTR;
	switch (c){
		case 0xe6: // Zero Page
					MPTR=&memory[AM_ZERO_PAGE()]; break;
		case 0xf6: // Zero Page,X
					MPTR=&memory[AM_ZP_X()]; break;
		case 0xee: // Absolute
					MPTR=&memory[AM_ABSOLUTE()]; break;
		case 0xfe: // Absolute,X
					MPTR=&memory[AM_ABS_X()]; break;
		default: break;
	}
	(*MPTR)++;
	if (((*MPTR) >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Negative Sign bit
	if ((*MPTR)==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if A=0 else clear
	return 0;
}  // End INC

int I_JMP(uchar c){  // completed
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
					CPU_A=memory[AM_IMMEDIATE()];
					break;
		case 0xA5: // Zero Page
					CPU_A=memory[AM_ZERO_PAGE()];
					break;
		case 0xB5: // Zero Page,X
					CPU_A=memory[AM_ZP_X()];
					break;
		case 0xAd:  // Absolute
					CPU_A=memory[AM_ABSOLUTE()];
					break;
		case 0xbd:  // Absolute,X
					CPU_A=memory[AM_ABS_X()];
					break;
		case 0xb9:  // Absolute,Y
					CPU_A=memory[AM_ABS_Y()];
					break;
		case 0xa1:  // Indirext,X
					CPU_A=memory[AM_IND_X()];
					break;
		case 0xb1: // Indirext,Y
					CPU_A=memory[AM_IND_Y()];
					break;
		default: printf("!!!!!\n");
	}
	SET_SR(0); // Set SR based on CPU_A
	return 0;
}  // End LDA

int I_LDX(uchar c){  // Completed
	switch (c){
		case 0xa2: // Immediate
					CPU_X=memory[AM_IMMEDIATE()];
					break;
		case 0xa6: // Zero Page
					CPU_X=memory[AM_ZERO_PAGE()];
					break;
		case 0xb6: // Zero Page,Y
					CPU_X=memory[AM_ZP_Y()];
					break;
		case 0xae: // Absolute
					CPU_X=memory[AM_ABSOLUTE()];
					break;
		case 0xbe:  // Absolute,Y
					CPU_X=memory[AM_ABS_Y()];
					break;
		default: break;
	}
	SET_SR(1);  // Set SR based on CPU_X
	return 0;
}  // End LDX

int I_LDY(uchar c){  // Completed
	switch (c){
		case 0xa0: // Immediate
					CPU_Y=memory[AM_IMMEDIATE()];
					break;
		case 0xa4: // Zero Page
					CPU_Y=memory[AM_ZERO_PAGE()];
					break;
		case 0xb4: // Zero Page,X
					CPU_Y=memory[AM_ZP_X()];
					break;
		case 0xac: // Absolute
					CPU_Y=memory[AM_ABSOLUTE()];
					break;
		case 0xbc: // Absolute,X
					CPU_Y=memory[AM_ABS_X()];
					break;
		default: printf("!!!!!\n");
	}
	SET_SR(2); // Set SR Flag based on CPU_Y
	return 0;
}  // End LDY

int I_LSR(uchar c){  // Completed untested
/*
 * Logical Shift Right all bits by 1, setting Carry to LSB (bit 0)
 * Set MSB (bit 7) to 0
 * 
 * it makes sense here to use pointers, as we need to manipulate
 * more than one type of entity (Accumulator or Memory).
 */
	uchar C,*MPTR;
	switch (c){
		case 0x4a: // Accumulator
					MPTR=&CPU_A; break;
		case 0x46: // Zero Page
					MPTR=&memory[AM_ZERO_PAGE()]; break;
		case 0x56: // Zero Page,X
					MPTR=&memory[AM_ZP_X()]; break;
		case 0x4e: // Absolute
					MPTR=&memory[AM_ABSOLUTE()]; break;
		case 0x5e: // Absolute,X
					MPTR=&memory[AM_ABS_X()]; break;
		default: break;
	}	
	CPU_SR|=(*MPTR)&1; // Set carry from Right most Bit
	(*MPTR)=(*MPTR)>>1; // Right Shift	
	return 0;
}  // End LSR

int I_NOP(uchar c){	return 0;} // Do Nothing

int I_ORA(uchar c){  // Completed Untested
	uchar Temp;
	switch (c){
		case 0x09: // Immediate ORA #$nn
					CPU_A|=memory[AM_IMMEDIATE()];
					break;
		case 0x05: // Zero Page
					CPU_A|=memory[AM_ZERO_PAGE()];
					break;
		case 0x15: // Zero Page,X
					CPU_A|=memory[AM_ZP_X()];
					break;
		case 0x0d: // Absolute
					CPU_A|=memory[AM_ABSOLUTE()];
					break;
		case 0x1d: // Absolute,X
					CPU_A|=memory[AM_ABS_X()];
					break;
		case 0x19: // Absolute,Y
					CPU_A&=memory[AM_ABS_Y()];
					break;
		case 0x01: // Indirect,X
					CPU_A|=memory[AM_IND_X()];
					break;
		case 0x11: // Indirect,Y
					CPU_A|=memory[AM_IND_Y()];
					break;
		default: printf("!!!!!\n");
	}
	SET_SR(0); // Set Flags based on CPU_A
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
	SET_SR(flag);
	return 0;
}  // End REG

int I_ROL(uchar c){
/*
 * Rotate Left. Shift all bits Left.
 * Original Carry goes into bit 0
 * Bit 7 goes into Carry
 * This creates a rotation effect in a loop
 */
	uchar C,*MPTR; // Carry and Memory/Accumulator Pointer
	switch (c){
		case 0x1a: // Accumulator
					MPTR=&CPU_A; break;
		case 0x26: // Zero Page
					MPTR=&memory[AM_ZERO_PAGE()]; break;
		case 0x36: // Zero Page,X
					MPTR=&memory[AM_ZP_X()]; break;
		case 0x2e: // Absolute
					MPTR=&memory[AM_ABSOLUTE()]; break;
		case 0x3e: // Absolute,X
					MPTR=&memory[AM_ABS_X()]; break;
		default: break;
	}
	C=CPU_SR&1; // Copy Carry to Temp Carry
	CPU_SR|=(*MPTR)>>7; // Set carry from Left most Bit
	(*MPTR)=(*MPTR)<<1; // Left Shift
	(*MPTR)&=C; // Set bit 0
	if (((*MPTR) >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Negative Sign bit
	if ((*MPTR)==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if A=0 else clear
	return 0;
}  // End ROL

int I_ROR(uchar c){ // Completed untested
/*
 * Rotate Right. Shift all bits Right.
 * Original Carry goes into bit 7
 * Bit 0 goes into Carry
 * This creates a rotation effect in a loop
 */
	uchar C,*MPTR; // Carry and Memory/Accumulator Pointer
	switch (c){
		case 0x6a: // Accumulator
					MPTR=&CPU_A; break;
		case 0x66: // Zero Page
					MPTR=&memory[AM_ZERO_PAGE()]; break;
		case 0x76: // Zero Page,X
					MPTR=&memory[AM_ZP_X()]; break;
		case 0x6e: // Absolute
					MPTR=&memory[AM_ABSOLUTE()]; break;
		case 0x7e: // Absolute,X
					MPTR=&memory[AM_ABS_X()]; break;
		default: break;
	}
	C=CPU_SR&1; // Copy Carry to Temp Carry
	(~((*MPTR)&1)) ? (CPU_SR&=~1) : (CPU_SR|=1); 
	(*MPTR)=(*MPTR)>>1; // Right Shift
	(*MPTR)|=(C<<7); // Set bit 7 from original Carry bit
	if (((*MPTR) >>7)&1)  CPU_SR|=128; else CPU_SR&=~128; // Set or clear Negative Sign bit
	if ((*MPTR)==0) CPU_SR|=2; else CPU_SR&=~2; // Set Zero Flag if A=0 else clear		
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

int I_STA(uchar c){  // Completed untested
	printf("STA\t");
	switch (c){
		case 0x85: // Zero Page
					memory[AM_ZERO_PAGE()]=CPU_A;
					break;
		case 0x95: // Zero Page,X
					memory[AM_ZP_X()]=CPU_A;
					break;
		case 0x8d: // Absolute
					memory[AM_ABSOLUTE()]=CPU_A;
					break;
		case 0x9d: // Absolute,X
					memory[AM_ABS_X()]=CPU_A;
					break;
		case 0x99: // Absolute,Y
					memory[AM_ABS_Y()]=CPU_A;
					break;
		case 0x81:// Indirect,X
					memory[AM_IND_X()]=CPU_A;
					break;
		case 0x91: // Indirect,Y
					memory[AM_IND_Y()]=CPU_A;
					break;
		default: break;
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

int I_STX(uchar c){  // Completed untested
	printf("STX\t");
	switch (c){
		case 0x86: // Zero Page
					memory[AM_ZERO_PAGE()]=CPU_X;
					break;
		case 0x96: // Zero Page,Y
					memory[AM_ZP_Y()]=CPU_X;
					break;
		case 0x8e: // ABSOLUTE
					memory[AM_ABSOLUTE()]=CPU_X;
					break;
		default: printf("!!!!!\n");
	}		
	return 0;
}  // End STX

int I_STY(uchar c){  // Completed untested
	printf("STY\t");
	switch (c){
		case 0x84: // ABSOLUTE
					memory[AM_ZERO_PAGE()]=CPU_Y;
					break;
		case 0x94: // ZERO PAGE,x
					memory[AM_ZP_X()]=CPU_Y;
					break;
		case 0x8c: // ABSOLUTE
					memory[AM_ABSOLUTE()]=CPU_Y;
					break;
		default: printf("!!!!!\n");
	}			
	return 0;
}  // End STY



// These are the Illegal or undefined instructions
int I_UDEF(uchar c){
	printf("...\n");
	
	return 0;
}  // End of I_UDEF
