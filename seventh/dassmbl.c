#include "fsm.h"


// This is a data structure for Referencing A when B=0-7 and C=0
// The arrary is organised in B Rows by A columns; C will always = 0.
// To make indexing easier, I have included unused rows for instrucitons that don't get indexed
// during the if..then..else..switch..case statements
char bc0[][4]={"BRK\0","...\0","RTI\0","RTS\0","...\0","...\0","...\0","...\0", // B=0 A=0-7 Odd instructions
				"...\0","...\0","...\0","...\0","...\0","...\0","...\0","...\0", // Unused
			"PHP\0","PLP\0","PHA\0","PLA\0","DEY\0","TAY\0","INY\0","INX\0", // B=2 A=0-7 Implied
				"...\0","...\0","...\0","...\0","...\0","...\0","...\0","...\0", // Unused
			"BPL\0","BMI\0","BVC\0","BVS\0","BCC\0","BCS\0","BNE\0","BEQ\0", // B=4 A=0-7 Branch Instructions
				"...\0","...\0","...\0","...\0","...\0","...\0","...\0","...\0", // Unused
			"CLC\0","SEC\0","CLI\0","SEI\0","TYA\0","CLV\0","CLD\0","SED\0",	 // B=6 A=0-7 Flag instructions
			"ASL\0","ROL\0","LSR\0","ROR\0","TXA\0","TAX\0","DEX\0","NOP\0"		// fudged values for strange instructions
			};
	
// Array organised C rows by A columns
// So to increment C would mean (array-elemnet*8)+A
// These are all the ordinary organised instructions where B=addressing mode and
// A,C = Instruction mnemonic.
char C_A[][4]={	"...\0","BIT\0","JMP\0","JMP\0","STY\0","LDY\0","CPY\0","CPX\0",	// C=0
				"ORA\0","AND\0","EOR\0","ADC\0","STA\0","LDA\0","CMP\0","SBC\0",	// C=1
				"ASL\0","ROL\0","LSR\0","ROR\0","STX\0","LDX\0","DEC\0","INC\0"};	// C=2


void DISASSEMBLE(char **tokens,int j){
	/*
	 * I used the 6502 reference manual Appendix C to decode the instructions.
	 * This is a technically much better approach, as well as shorter code.
	 * 
	 * The way to think of this is as Octal.  AAABBBCC.
	 * With a couple of exceptions, if the low bit of B AND C are both 0 then the instruction
	 * can be thought of as a single 8bit entity; implied or accumulator.
	 * 
	 * The exceptions to this are JSR, Branch when B=4 and the 3 immediate instructions in B=0 when A=5-7
	 * 
	 * I am not sure I can re-use the data structures (arrays) for assemblling, but
	 * I don't actually care at the moment.
	 */
	uchar INST,A=0,B=0,C=0;
	int base,insttype,index;
	unsigned short ci,end;
	(j<3)?(index=1):(index=2);
	if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
	ci=(int)strtol(tokens[1],NULL,base);  // convert string to integer
	end=(int)strtol(tokens[index],NULL,base);  // convert string to integer
	
	while (ci<end+1){
		INST=memory[ci++];
		wprintw(main_win,"$%04x\t",ci-1); // print address in hex followed by tab
		A=(INST>>5)&7; // Mask 3 bits so A=0-7
		B=(INST>>2)&7; // Mask 3 bits so B=0-7
		C=INST&3; // Mask 2 bits so C=0-3
		if (((C==0) || (C==2)) 		// is the LSB of C = 0, ie are we on row 0 or 2?
			&& ((B==2) || (B==6) || (B==0 // Does b have a 0 as LSB? Excluding Branches
			&& ((A==0) || (A==2) || (A==3) // b=0 only matters if A is 0,2 or 3
			)))) // then we have an implied instructions
			{	(C==2&&B==2)?	// Everything else must be correct, so check specifically for C=2,B=2
				(wprintw(main_win,"%02x\t\t%s\n",INST,bc0[56+A]))
				:	// else C must be 0 and primary conditions for B and A must be met.
				(wprintw(main_win,"%02x\t\t%s\n",INST,bc0[(B*8)+A]));
		} else  // End dealing with implied and accumulator instructions

		if ((C==0)&&(B==0)&&(A==1)) // deal with JSR
			{wprintw(main_win,"%02x %02x %02x\tJSR $%02x%02x\n",INST,memory[ci-2],memory[ci-1],memory[ci++],memory[ci++]);

		} else // Not a JSR, deal with Branches next
		if ((C==0) && (B==4)) // Branches
			{wprintw(main_win,"%02x %02x\t\t%s $%02x\n",INST,memory[ci-1],bc0[(B*8)+A],memory[ci++]);
		} else // Not a branch, then must be everything else
			{
				switch(B) { // now we can use B as a reference to addressing mode
					case 0: // x,indirect and remaining immediate instructions
							((A==5&&C==2)||(((A>>2)&1)&&C==0||C==2))
							? (wprintw(main_win,"%02x %02x\t\t%s #$%02x\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++])) // Immediate
							: (wprintw(main_win,"%02x %02x\t\t%s ($%02x,X)\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++])); // Indirect,X
							break;
					case 1: // Zero Page
							wprintw(main_win,"%02x %02x\t\t%s $%02x\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++]); // ZP
							break;
					case 2: // Immediate
							wprintw(main_win,"%02x %02x\t\t%s #$%02x\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++]); // Immediate
							break;
					case 3: // Absolute
							(A==3)?( // deal with Indirect JMP
							wprintw(main_win,"%02x %02x %02x\t%s ($%02x%02x)\n",INST,memory[ci-2],memory[ci-1],C_A[A+(C*8)],memory[ci++],memory[ci++]))
							: // else not indirect JMP
							(wprintw(main_win,"%02x %02x %02x\t%s $%02x%02x\n",INST,memory[ci-2],memory[ci-1],C_A[A+(C*8)],memory[ci++],memory[ci++]));
							break;
					case 4: // Indirect,Y
							wprintw(main_win,"%02x %02x\t\t%s ($%02x),Y\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++]);
							break;
					case 5: // Zero Page,X
							wprintw(main_win,"%02x %02x\t\t%s $%02x,X\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++]);
							break;
					case 6: // Absolute,Y
							wprintw(main_win,"%02x %02x %02x\t%s $%02x%02x,Y\n",INST,memory[ci-2],memory[ci-1],C_A[A+(C*8)],memory[ci++],memory[ci++]);
							break;
					case 7: // Absolute,X
							wprintw(main_win,"%02x %02x %02x\t%s $%02x%02x,X\n",INST,memory[ci-2],memory[ci-1],C_A[A+(C*8)],memory[ci++],memory[ci++]);
							break;
					default: wprintw(main_win,"???\n");break;
			} // end switch

		} // End if conditions
		wrefresh(main_win);
	} // End While
	
} // End DISASSEMBLE_NEW



