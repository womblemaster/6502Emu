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
	int base,insttype;
	unsigned short ci,end;
	if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
	ci=(int)strtol(tokens[1],NULL,base);  // convert string to integer
	end=(int)strtol(tokens[2],NULL,base);  // convert string to integer
	
	while (ci<end+1){
		INST=memory[ci++];
		printf("$%04x\t",ci-1); // print address in hex followed by tab
		A=(INST>>5)&7; // Mask 3 bits so A=0-7
		B=(INST>>2)&7; // Mask 3 bits so B=0-7
		C=INST&3; // Mask 2 bits so C=0-3
		if (((C==0) || (C==2)) 		// is the LSB of C = 0, ie are we on row 0 or 2?
			&& ((B==2) || (B==6) || (B==0 // Does b have a 0 as LSB? Excluding Branches
			&& ((A==0) || (A==2) || (A==3) // b=0 only matters if A is 0,2 or 3
			)))) // then we have an implied instructions
			{	(C==2&&B==2)?
				(printf("%02x\t\t%s\n",INST,bc0[56+A]))
				:
				(printf("%02x\t\t%s\n",INST,bc0[(B*8)+A]));
		} else  // End dealing with implied and accumulator instructions

		if ((C==0)&&(B==0)&&(A==1)) // deal with JSR
			{printf("%02x %02x %02x\tJSR $%02x%02x\n",INST,memory[ci-2],memory[ci-1],memory[ci++],memory[ci++]);

		} else // Not a JSR, deal with Branches next
		if ((C==0) && (B==4)) // Branches
			{printf("%02x %02x\t\t%s $%02x\n",INST,memory[ci-1],bc0[(B*8)+A],memory[ci++]);
		} else // Not a branch, then must be everything else
			{
				switch(B) { // now we can use B as a reference to addressing mode
					case 0: // x,indirect and remaining immediate instructions
							((A==5&&C==2)||(((A>>2)&1)&&C==0||C==2))
							? (printf("%02x %02x\t\t%s #$%02x\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++])) // Immediate
							: (printf("%02x %02x\t\t%s ($%02x,X)\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++])); // Indirect,X
							break;
					case 1: // Zero Page
							printf("%02x %02x\t\t%s $%02x\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++]); // ZP
							break;
					case 2: // Immediate
							printf("%02x %02x\t\t%s #$%02x\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++]); // Immediate
							break;
					case 3: // Absolute
							(A==3)?( // deal with Indirect JMP
							printf("%02x %02x %02x\t%s ($%02x%02x)\n",INST,memory[ci-2],memory[ci-1],C_A[A+(C*8)],memory[ci++],memory[ci++]))
							: // else not indirect JMP
							(printf("%02x %02x %02x\t%s $%02x%02x\n",INST,memory[ci-2],memory[ci-1],C_A[A+(C*8)],memory[ci++],memory[ci++]));
							break;
					case 4: // Indirect,Y
							printf("%02x %02x\t\t%s ($%02x),Y\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++]);
							break;
					case 5: // Zero Page,X
							printf("%02x %02x\t\t%s $%02x,X\n",INST,memory[ci-1],C_A[A+(C*8)],memory[ci++]);
							break;
					case 6: // Absolute,Y
							printf("%02x %02x %02x\t%s $%02x%02x,Y\n",INST,memory[ci-2],memory[ci-1],C_A[A+(C*8)],memory[ci++],memory[ci++]);
							break;
					case 7: // Absolute,X
							printf("%02x %02x %02x\t%s $%02x%02x,X\n",INST,memory[ci-2],memory[ci-1],C_A[A+(C*8)],memory[ci++],memory[ci++]);
							break;
					default: printf("???\n");break;
			} // end switch

		} // End if conditions

	} // End While
	
} // End DISASSEMBLE_NEW



void DISASSEMBLE_OLD(char **tokens,int j){
/*
 * This code is a cludge approach, there is a better way that I need
 * to fathom properly.  I needed a quick and dirty disassembler, so
 * switch and case do the trick for the time being.
 * 
 * It appears 6502 instructions are encoded into parts; such as
 * AAABBBCC;
 * Where CC = Class of instruction to run; how many addressing modes it has etc
 * BBB = The addressing mode in the class.
 * AAA = Instruction applicable within the classification.
 * 
 * But, different instruction groups use different decoding classes
 * I need to work out how to decode all of these, so that they present
 * the correct mnemonic etc.  There is a program called SuperMon from
 * the early days of the C64, plus the AppleII monitor which uses this
 * approach.  I will try and implement something based on this, as I can
 * then use the same approach to implement the assembler.
 */
	uchar INST,A=0,B=0,C=0;
	int base;
	unsigned short ci,end;
	if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
	ci=(int)strtol(tokens[1],NULL,base);  // convert string to integer
	end=(int)strtol(tokens[2],NULL,base);  // convert string to integer

	printf("\nADDR\tHEX\tMNEMONIC\n");

	while (ci<end+1){
		INST=memory[ci];
		printf("%04x\t",ci++);
		switch (INST){
			case 0x00: printf("%02x\tBRK\n",INST); break;

			// ADC
			case 0x69:	printf("%02x %02x\tADC #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0x65:	printf("%02x %02x\tADC $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x75:	printf("%02x %02x\tADC $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x6d:	printf("%02x %02x%02x\tADC $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute				
			case 0x7d:	printf("%02x %02x%02x\tADC $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X				
			case 0x79:	printf("%02x %02x%02x\tADC $%02x%02x,Y\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,Y			
			case 0x61:	printf("%02x %02x\tADC ($%02x,X)\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,X			
			case 0x71:	printf("%02x %02x\tADC ($%02x),Y\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,Y

			// AND
			case 0x29:	printf("%02x %02x\tAND #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0x25:	printf("%02x %02x\tAND $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x35:	printf("%02x %02x\tAND $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x2d:	printf("%02x %02x%02x\tAND $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute				
			case 0x3d:	printf("%02x %02x%02x\tAND $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X				
			case 0x39:	printf("%02x %02x%02x\tAND $%02x%02x,Y\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,Y			
			case 0x21:	printf("%02x %02x\tAND ($%02x,X)\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,X			
			case 0x31:	printf("%02x %02x\tAND ($%02x),Y\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,Y

			// ASL
			case 0x0a:	printf("%02x  \tASL \n",INST);break; // Accumulator	
			case 0x06:	printf("%02x %02x\tASL $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP
			case 0x16:	printf("%02x %02x\tASL $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X
			case 0x0e:	printf("%02x %02x%02x\tASL $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute
			case 0x1e:	printf("%02x %02x%02x\tASL $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X
			
			// BIT
			case 0x24:	printf("%02x %02x\tBIT $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP
			case 0x2c:	printf("%02x %02x%02x\tBIT $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute

			// Branches
			case 0x10: printf("%02x %02x\tBPL $%02x\n",INST,memory[ci-1],memory[ci++]); break;
			case 0x30: printf("%02x %02x\tBMI $%02x\n",INST,memory[ci-1],memory[ci++]); break;
			case 0x50: printf("%02x %02x\tBVC $%02x\n",INST,memory[ci-1],memory[ci++]); break;
			case 0x70: printf("%02x %02x\tBVS $%02x\n",INST,memory[ci-1],memory[ci++]); break;
			case 0x90: printf("%02x %02x\tBCC $%02x\n",INST,memory[ci-1],memory[ci++]); break;
			case 0xB0: printf("%02x %02x\tBCS $%02x\n",INST,memory[ci-1],memory[ci++]); break;
			case 0xD0: printf("%02x %02x\tBNE $%02x\n",INST,memory[ci-1],memory[ci++]); break;
			case 0xF0: printf("%02x %02x\tBEQ $%02x\n",INST,memory[ci-1],memory[ci++]); break;
			
			
			// CMP
			case 0xc9:	printf("%02x %02x\tCMP #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0xc5:	printf("%02x %02x\tCMP $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0xd5:	printf("%02x %02x\tCMP $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0xcd:	printf("%02x %02x%02x\tCMP $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute				
			case 0xdd:	printf("%02x %02x%02x\tCMP $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X				
			case 0xd9:	printf("%02x %02x%02x\tCMP $%02x%02x,Y\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,Y			
			case 0xc1:	printf("%02x %02x\tCMP ($%02x,X)\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,X			
			case 0xd1:	printf("%02x %02x\tCMP ($%02x),Y\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,Y
			
			// CPX
			case 0xe0:	printf("%02x %02x\tCPX #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate
			case 0xe4:	printf("%02x %02x\tCPX $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP	
			case 0xec:	printf("%02x %02x%02x\tCPX $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute
			
			// CPY
			case 0xc0:	printf("%02x %02x\tCPX #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate
			case 0xc4:	printf("%02x %02x\tCPX $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP	
			case 0xcc:	printf("%02x %02x%02x\tCPX $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute
			
			// DEC
			case 0xc6:	printf("%02x %02x\tDEC $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP					
			case 0xd6:	printf("%02x %02x\tDEC $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X					
			case 0xce:	printf("%02x %02x%02x\tDEC $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute		
			case 0xde:  printf("%02x %02x%02x\tDEC $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X	
			
			// EOR
			case 0x49:	printf("%02x %02x\tEOR #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0x45:	printf("%02x %02x\tEOR $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x55:	printf("%02x %02x\tEOR $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x4d:	printf("%02x %02x%02x\tEOR $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute				
			case 0x5d:	printf("%02x %02x%02x\tEOR $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X				
			case 0x59:	printf("%02x %02x%02x\tEOR $%02x%02x,Y\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,Y			
			case 0x41:	printf("%02x %02x\tEOR ($%02x,X)\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,X			
			case 0x51:	printf("%02x %02x\tEOR ($%02x),Y\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,Y
			
			// INC
			case 0xe6:	printf("%02x %02x\tINC $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0xf6:  printf("%02x %02x\tINC $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0xee:  printf("%02x %02x%02x\tINC $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute		
			case 0xfe:  printf("%02x %02x%02x\tINC $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X	
			
			// JMP
			case 0x4c:	printf("%02x %02x%02x\tJMP $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute
			case 0x6c:	printf("%02x %02x%02x\tJMP ($%02x%02x)\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute
			
			// JSR
			case 0x20: printf("%02x %02x%02x\tJSR $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]); break;
			
			//LDA
			case 0xa9:	printf("%02x %02x\tLDA #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0xa5:	printf("%02x %02x\tLDA $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0xb5:	printf("%02x %02x\tLDA $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0xad:	printf("%02x %02x%02x\tEOR $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute				
			case 0xbd:	printf("%02x %02x%02x\tEOR $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X				
			case 0xb9:	printf("%02x %02x%02x\tEOR $%02x%02x,Y\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,Y			
			case 0xa1:	printf("%02x %02x\tLDA ($%02x,X)\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,X			
			case 0xb1:	printf("%02x %02x\tLDA ($%02x),Y\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,Y
			
			// LDX
			case 0xa2:	printf("%02x %02x\tLDX #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0xa6:  printf("%02x %02x\tLDX $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0xb6:  printf("%02x %02x\tLDX $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0xae:  printf("%02x %02x%02x\tLDX $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute		
			case 0xbe:  printf("%02x %02x%02x\tLDX $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X
			
			// LDY
			case 0xa0:	printf("%02x %02x\tLDY #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0xa4:  printf("%02x %02x\tLDY $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0xb4:  printf("%02x %02x\tLDY $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0xac:  printf("%02x %02x%02x\tLDY $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute		
			case 0xbc:  printf("%02x %02x%02x\tLDY $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X
			
			// LSR
			case 0x4a:	printf("%02x %02x\tLSR\n",INST,memory[ci++]);break; // Accumulator			
			case 0x46:  printf("%02x %02x\tLSR $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x56:  printf("%02x %02x\tLSR $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x4e:  printf("%02x %02x%02x\tLSR $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute		
			case 0x5e:	printf("%02x %02x%02x\tLSR $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X		
			
			// ORA
			case 0x09:	printf("%02x %02x\tORA #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0x05:	printf("%02x %02x\tORA $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x15:	printf("%02x %02x\tORA $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x0d:	printf("%02x %02x%02x\tORA $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute			
			case 0x1d:	printf("%02x %02x%02x\tORA $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X		
			case 0x19:	printf("%02x %02x%02x\tORA $%02x%02x,Y\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,Y		
			case 0x01:	printf("%02x %02x\tORA ($%02x,X)\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,X			
			case 0x11:	printf("%02x %02x\tORA ($%02x),Y\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,Y
			
			// ROL
			case 0x1a:	printf("%02x %02x\tROL\n",INST,memory[ci++]);break; // Accumulator			
			case 0x26:  printf("%02x %02x\tROL $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x36:  printf("%02x %02x\tROL $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x2e:  printf("%02x %02x%02x\tROL $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute		
			case 0x3e:  printf("%02x %02x%02x\tROL $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X	
			
			// ROR
			case 0x6a:	printf("%02x %02x\tROR\n",INST,memory[ci++]);break; // Accumulator			
			case 0x66:  printf("%02x %02x\tROR $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x76:  printf("%02x %02x\tROR $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x6e:   printf("%02x %02x%02x\tROR $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute		
			case 0x7e:  printf("%02x %02x%02x\tROR $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X	
			
			// SBC
			case 0xE9:	printf("%02x %02x\tSBC #$%02x\n",INST,memory[ci-1],memory[ci++]);break; // Immediate			
			case 0xE5:	printf("%02x %02x\tSBC $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0xF5:	printf("%02x %02x\tSBC $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0xed:	printf("%02x %02x%02x\tSBC $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute			
			case 0xfd:	printf("%02x %02x%02x\tSBC $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X		
			case 0xf9:	printf("%02x %02x%02x\tSBC $%02x%02x,Y\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,Y		
			case 0xe1:	printf("%02x %02x\tSBC ($%02x,X)\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,X			
			case 0xf1:	printf("%02x %02x\tSBC ($%02x),Y\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,Y
			
			// STA
			case 0x85:	printf("%02x %02x\tSTA $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x95:  printf("%02x %02x\tSTA $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x8d:  printf("%02x %02x%02x\tSTA $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute		
			case 0x9d:  printf("%02x %02x%02x\tSTA $%02x%02x,X\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,X	
			case 0x99:  printf("%02x %02x%02x\tSTA $%02x%02x,Y\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; // Absolute,Y	
			case 0x81:  printf("%02x %02x\tSTA ($%02x,X)\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,X			
			case 0x91:  printf("%02x %02x\tSTA ($%02x),Y\n",INST,memory[ci-1],memory[ci++]);break; // Indirect,Y
			
			// STX
			case 0x86:	printf("%02x %02x\tSTX $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x96:  printf("%02x %02x\tSTX $%02x,Y\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x8e:  printf("%02x %02x%02x\tSTX $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute	
			
			// STY
			case 0x84:	printf("%02x %02x\tSTY $%02x\n",INST,memory[ci-1],memory[ci++]);break;  // ZP				
			case 0x94:  printf("%02x %02x\tSTY $%02x,X\n",INST,memory[ci-1],memory[ci++]);break; //ZP,X				
			case 0x8c:  printf("%02x %02x%02x\tSTY $%02x%02x\n",INST,memory[ci-1],memory[ci+1],memory[ci++],memory[ci++]);break; //Absolute	

          	// Single byte OP Codes	
			case 0x40: printf("%02x\tRTI\n",INST); break;
			case 0x60: printf("%02x\tRTS\n",INST); break;
			case 0x08: printf("%02x\tPHP\n",INST); break;
			case 0x28: printf("%02x\tPLP\n",INST); break;
			case 0x48: printf("%02x\tPHA\n",INST); break;
			case 0x68: printf("%02x\tPLA\n",INST); break;
			case 0x88: printf("%02x\tDEY\n",INST); break;
			case 0xA8: printf("%02x\tTAY\n",INST); break;
			case 0xC8: printf("%02x\tINY\n",INST); break;
			case 0xE8: printf("%02x\tINX\n",INST); break;
			case 0x18: printf("%02x\tCLC\n",INST); break;
			case 0x38: printf("%02x\tSEC\n",INST); break;
			case 0x58: printf("%02x\tCLI\n",INST); break;
			case 0x78: printf("%02x\tSEI\n",INST); break;
			case 0x98: printf("%02x\tTYA\n",INST); break;
			case 0xB8: printf("%02x\tCLV\n",INST); break;
			case 0xD8: printf("%02x\tCLD\n",INST); break;
			case 0xF8: printf("%02x\tSED\n",INST); break;
			case 0x8A: printf("%02x\tTXA\n",INST); break;
			case 0x9A: printf("%02x\tTXS\n",INST); break;
			case 0xAA: printf("%02x\tTAX\n",INST); break;
			case 0xBA: printf("%02x\tTSX\n",INST); break;
			case 0xCA: printf("%02x\tDEX\n",INST); break;
			case 0xEA: printf("%02x\tNOP\n",INST); break;
			
			default: printf("???\n"); break;
		}

	}

}
