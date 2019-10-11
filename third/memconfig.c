/*
 * Created this file to allow simple configuration of memory.
 * 
 * We can input values into memory and then execute them, proving that
 * the instruction code works as expected.
 */


#include "fsm.h"

void BOARD_MEM_CONFIG(){
	
	// Setup Memory for testing
	// ROM vectors first
	memory[0xfffc]=0x00;
	memory[0xfffd]=0xc0;
	memory[0xfffe]=0x00;
	memory[0xffff]=0xff;
	
	// Test OP codes
	memory[0xc000]=0x20; // JSR
	memory[0xc001]=0xa0; // Low byte
	memory[0xc002]=0xb0; // high btye: address b0a0
	memory[0xb000]=0x60; // RTS from previous JSR	

	memory[0xc000]=0x2d; // AND Absolute
	memory[0xc001]=0xa0; // Low byte
	memory[0xc002]=0xb0; // high btye: address b0a0
	memory[0xb0a0]=0xaa; // Value for AND
	
	memory[0xb100]=0x6c; // Indirect JMP
	memory[0xb101]=0x00; // Low byte
	memory[0xb102]=0xb2; // High byte
	memory[0xb200]=0xfe; // Low byte
	memory[0xb201]=0xfc; // high byte PC should read fcfe after this lot
		
} // End BOARD_MEM_CONFIG
