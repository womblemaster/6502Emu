/*
 * The goal is to prove the instruction set is identified properly
 * 
 * RAM is emulated by an array memory[] of size 65536 unsigned bytes
 * 
 * SYS_RESET puts every 8 bit hex value from 00 to FF into the memory
 * between 100 and 1FF.  This emulates every 6502 legal and illegal OP code.
 * 
 * The main() for{} loop then loops from 0x100(256 decimal) to 0x1FF(511 decimal)
 * reads the hex value in memory 00-FF and using the array of functions defined
 * in fsm.h, calls the routine using the loop counter-256 as the index.
 * 
 * The individual functions then take the hex OP code a as parameter and uses
 * this to select what to print.  This way, all OP codes and addressing modes
 * can be shown to function.
 * 
 * 
 * Currently we don't care about illegal OP codes, we can add them later if
 * this goes further than just a basic emulator.
 * 
 * 
 */


#include "fsm.h"
#include <stdio.h>



int main(){
	int result,j;

	result=SYS_RESET();

	for(j=256;j<512;j++){
		printf("%d:\t%x\t",j,memory[j]);
		result=(*instfunc[j-256])(j-256);
	}

	result=CLEAN_UP();
	return 0;
}

int SYS_RESET(){

	FILE *fp;
	int mp=0xe000;
	// Set Address 0 to 0101111 (0x2F)
	memory[0]=0x2f;
	int j;

	// Set Memory range 256-511 to represent 0-255 base 10
	for (j=256;j<512;j++){
		memory[j]=j-256;
	}


	return 0;
}

int CLEAN_UP(){	printf("Clean UP\n");}


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

int I_AND(uchar c){
	printf("AND \t");
	switch (c){
		case 0x29: printf("Immediate\tOP A   \tL 2\tT 2\n"); break;
		case 0x25: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x35: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0x2d: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0x3d: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		case 0x39: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		case 0x21: printf("x,indirect\tOP ($nn,x)\tL 2\tT 6\n"); break;
		case 0x31: printf("Y,indrect\tOP ($nn),y\tL 2\tT 5\n"); break;
		default: printf("!!!!!\n");
	}
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


int I_BRK(unsigned char c){
	printf("BRK\n");
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


int I_EOR(uchar c){
	printf("EOR\t");
	switch (c){
		case 0x49: printf("Immediate\tOP #$nn\tL 2\tT 2\n"); break;
		case 0x45: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x55: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0x4d: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0x5d: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		case 0x59: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		case 0x41: printf("x,indirect\tOP ($nn,x)\tL 2\tT 6\n"); break;
		case 0x51: printf("Y,indrect\tOP ($nn),y\tL 2\tT 5\n"); break;
		default: printf("!!!!!\n");
	}
	return 0;
}  // End EOR


int I_FLG(uchar c){	
	switch (c){
		case 0x18: printf("CLC\tClear Carry Flag\n"); break;
		case 0x38: printf("SEC\tSet Carry Flag\n"); break;
		case 0x58: printf("CLI\tClear Interupt Flag\n"); break;
		case 0x78: printf("SEI\tSet Interupt Flag\n"); break;
		case 0xb8: printf("CLV\tClear Overflow Flag\n"); break;
		case 0xd8: printf("CLD\tClear Decimal Flag\n"); break;
		case 0xf8: printf("SED\tSet Decimal Flag\n"); break;
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
	printf("JMP\t");
	switch (c){
		case 0x4c: printf("Absolute OP $nnnn\tL 3\tT 3\n"); break;
		case 0x6c: printf("indrect\tOP ($nnnn)\tL 3\tT 5\n"); break;
		default: printf("!!!!!\n");
	}
	return 0;
}  // End JMP

int I_JSR(uchar c){
	printf("JSR\tAbsolute only\t\t\tL 3\tT 6\n");
	
	return 0;
}  // End JSR

int I_LDA(uchar c){
	printf("LDA\t");
	switch (c){
		case 0xA9: printf("Immediate\tOP #$nn \tL 2\tT 2\n"); break;
		case 0xA5: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xB5: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0xAd: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0xbd: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		case 0xb9: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		case 0xa1: printf("x,indirect\tOP ($nn,x)\tL 2\tT 6\n"); break;
		case 0xb1: printf("Y,indrect\tOP ($nn),y\tL 2\tT 5\n"); break;
		default: printf("!!!!!\n");
	}	
	return 0;
}  // End LDA

int I_LDX(uchar c){
	printf("LDX\t");
	switch (c){
		case 0xa2: printf("Immediate\tOP #$nn \tL 2\tT 2\n"); break;
		case 0xa6: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xb6: printf("Zero Page,Y\tOP $nn,y\tL 2\tT 4\n"); break;
		case 0xae: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0xbe: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		default: printf("!!!!!\n");
	}
	return 0;
}  // End LDX

int I_LDY(uchar c){
	printf("LDY\t");
	switch (c){
		case 0xa0: printf("Immediate\tOP #$nn \tL 2\tT 2\n"); break;
		case 0xa4: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0xb4: printf("Zero Page,X\tOP $nn,y\tL 2\tT 4\n"); break;
		case 0xac: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0xbc: printf("Absolute,X\tOP $nnnn,Y\tL 3\tT 4\n"); break;
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

int I_NOP(uchar c){	printf("NOP\n");return 0;}

int I_ORA(uchar c){
	printf("ORA\t");
	switch (c){
		case 0x09: printf("Accumulator\tOP A   \tL 2\tT 2\n"); break;
		case 0x05: printf("Zero Page\tOP $nn  \tL 2\tT 3\n"); break;
		case 0x15: printf("Zero Page,X\tOP $nn,x\tL 2\tT 4\n"); break;
		case 0x0d: printf("Absolute\tOP $nnnn\tL 3\tT 4\n"); break;
		case 0x1d: printf("Absolute,X\tOP $nnnn,x\tL 3\tT 4\n"); break;
		case 0x19: printf("Absolute,Y\tOP $nnnn,Y\tL 3\tT 4\n"); break;
		case 0x01: printf("x,indirect\tOP ($nn,x)\tL 2\tT 6\n"); break;
		case 0x11: printf("Y,indrect\tOP ($nn),y\tL 2\tT 5\n"); break;
		default: printf("!!!!!\n");
	}
	return 0;
}  // End or I_ORA


int I_REG(uchar c){
	// All Implied instructions of 1 byte, 2 Cycles
	switch (c){
		case 0xAA: printf("TAX\tTransfer A to X\n"); break;
		case 0x8A: printf("TXA\tTransfer X to A\n"); break;
		case 0xCA: printf("DEX\tDecrement X by 1\n"); break;
		case 0xE8: printf("INX\tIncrement X by one\n"); break;
		case 0xA8: printf("TAY\tTransfer A to Y\n"); break;
		case 0x98: printf("TYA\tTransfer Y to A\n"); break;
		case 0x88: printf("DEY\tDecrement Y by 1\n"); break;
		case 0xC8: printf("INY\tIncrement Y by 1\n"); break;
		default: printf("!!!!!\n");
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

int I_RTI(uchar c){
	printf("RTI\n");
/*
 * Remove Status Word and PC from stack in that order
 * The return address is the actual address NOT address-1 as in RTS
 * */	
	return 0;
}  // End RTI

int I_RTS(uchar c){
	printf("RTS\n");
/*
 * Return address is address-1, so PC needs 1 adding to continue to next instruction
 * Pull 2 bytes from stack; low first
 * uchar temp=memory[0x1+--_SP]
 * _PC=temp
 * uchar temp=memory[0x1+--_SP
 */
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

int I_STC(uchar c){
	switch (c){
		case 0x9a: printf("TXS\tTransfer X to SP\n"); break;
		case 0xba: printf("TSC\tTransfer SP to X\n"); break;
		case 0x48: printf("PHA\tPush A to Stack\n"); break;
		case 0x68: printf("PLA\tPull A from Stack\n"); break;
		case 0x08: printf("PHP\tPush Processor to Status\n"); break;
		case 0x28: printf("PLP\tPull Processor Status\n"); break;
		default: printf("!!!!!\n");
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
