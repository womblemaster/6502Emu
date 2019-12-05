#include "fsm.h"

void SET_SR(char flag){
	switch (flag){  // Set SR depending on which Register was target
		case 0: // Working with A Register
				if ((CPU.A >>7)&1)  CPU.SR|=128; else CPU.SR&=~128; // Set or clear Negative Sign bit
				if (CPU.A==0) CPU.SR|=2; else CPU.SR&=~2; // Set Zero Flag if A=0 else clear
				break;
		case 1:	// Working with X Register
				if ((CPU.X >>7)&1)  CPU.SR|=128; else CPU.SR&=~128; // Set or clear Negative Sign bit
				if (CPU.X==0) CPU.SR|=2; else CPU.SR&=~2; // Set Zero Flag if X=0 else clear
				break;
		case 2: // Working with Y Register
				if ((CPU.Y >>7)&1)  CPU.SR|=128; else CPU.SR&=~128; // Set or clear Negative Sign bit
				if (CPU.Y==0) CPU.SR|=2; else CPU.SR&=~2; // Set Zero Flag if Y=0 else clear
				break;
		default:break;  // Never hit, as there are only 3 registers; and flag is initialised to zero
	} 	
}

ushort AM_IMMEDIATE(){  // Completed
/*
 * Return the 8bit Contents of the memory pointed to by PC
 * increment PC ready for next instruction
 */
	return CPU.PC++;
}  // End AM_IMMEDIATE


uchar AM_ZERO_PAGE(){  // Completed
/*
 * Return the 8bit contents of memory on Zero Page
 * Pointed to by the 8bit memory location currently pointed
 * to by the PC!!!
 */
	return (memory[CPU.PC++]);
}  // End AM_ZERO_PAGE


ushort AM_ABSOLUTE(){  // Completed
/*
 * Return the 8bit contents of memory anywhere
 * Pointed to by the 16bit value located in PC and PC+1
 * remember little endian, so low byte first.
 */
	unsigned short int Temp;
	Temp=memory[CPU.PC++];
	Temp|=(memory[CPU.PC++])<<8;
	return Temp;
}  // End AM_ABSOLUTE


uchar AM_ZP_X(){  // Completed
/*
 * Return address at $nn + X
 * Address wraps at FF, so $fd + 5  returns contents of
 * $02
 * 
 * Instructions are 2 bytes: OP + 1 byte ZP Address
 */
	uchar Temp;
	Temp=(memory[CPU.PC++]);
	return (Temp+CPU.X)&255;

}  // End AM_ZP_X


uchar AM_ZP_Y(){  // Completed
/*
 * Return address at $nn + Y
 * Address wraps at FF, so $fd + 5  returns contents of
 * $02
 * 
 * Instructions are 2 bytes: OP + 1 byte ZP Address
 */
	uchar Temp;
	Temp=(memory[CPU.PC++]);
	return (Temp+CPU.Y)&255;
}  // End AM_ZP_Y

ushort AM_ABS_X(){  // Completed
/*
 * Return the 8bit contents of memory anywhere
 * Pointed to by the 16bit value located in memory[PC => PC+1] + CPU.X
 * remember little endian, so low byte first.
 */
	unsigned short int Temp;
	Temp=memory[CPU.PC++];
	Temp|=(memory[CPU.PC++])<<8;
	return Temp+CPU.X;	
}  // End AM_ABS_X


ushort AM_ABS_Y(){  // Completed
/*
 * Return the 8bit contents of memory anywhere
 * Pointed to by the 16bit value located in memory[PC => PC+1] + CPU.Y
 * remember little endian, so low byte first.
 */
	unsigned short int Temp;
	Temp=memory[CPU.PC++];
	Temp|=(memory[CPU.PC++])<<8;
	return Temp+CPU.Y;	
}  // End AM_ABS_Y


ushort AM_IND_X(){
/*
 * This is indirect,x or indexed indirect (rubbish name)
 * for LDA ($10,x) which is represented in Hex as a1 10
 * use the $10 as ZP address and add CPU.X to get the low
 * byte of a 2 byte address somewhere in 16 byte memory range.
 */
	uchar Temp;
	unsigned short int Reference;
	Temp=memory[CPU.PC++];			// Get ZP address from Memory after OP Code
	Reference=memory[(Temp+CPU.X)];    // Temp=contents of location ZP+X		// Reference = low byte
	Reference|=(memory[++Temp])<<8;	// Rererence add high byte
	return Reference;		// Return contents of memory location Reference!
}  // End AM_IND_X

ushort AM_IND_Y(){  // Completed
/*
 * This is indexed indirect or indirext,y.
 * for LDA ($10),Y or b1 10 in Hex
 * use contents of ZP memory location $10 as low byte to index and contents of ZP+1 ($11) as high byte
 * if $10 contains d2 and $11 contained C2 then the base index is $c2d2
 * then if Y=3 the final address is $c2d2+Y or $c2d5.
 */
	uchar Temp;
	unsigned short int Reference;
	Temp=memory[CPU.PC++];
	Reference=memory[Temp++]; 		// Reference = low byte
	Reference|=(memory[Temp])<<8;	// Rererence add high byte
	return Reference+CPU.Y;		// Return contents of memory location Reference!
}  // End AM_IND_Y
