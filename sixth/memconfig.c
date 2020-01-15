#include "fsm.h"

void BOARD_MEM_CONFIG(){
	
	// Setup Memory for testing
	
	
// ROM vectors and code

		// Characters to print to screen memory
	memory[0xfe00]=0x2a; // *
	memory[0xfe01]=0x36; // 6
	memory[0xfe02]=0x35; // 5
	memory[0xfe03]=0x30; // 0
	memory[0xfe04]=0x32; // 2
	memory[0xfe05]=0x20; // Space
	memory[0xfe06]=0x45; // E
	memory[0xfe07]=0x4d; // M
	memory[0xfe08]=0x55; // U
	memory[0xfe09]=0x2a; // *

	memory[0xff00]=0x20; // JSR to initial screen output routine
	memory[0xff01]=0x10; // Lo byte
	memory[0xff02]=0xff; // hi byte
	memory[0xff03]=0x40; // RTI for BRK vector
	
		// Code to jump to Start of main program
	memory[0xff04]=0x20; // JSR to screen print
	memory[0xff05]=0x10;
	memory[0xff06]=0xff;
	memory[0xff07]=0x00; // BRK
	memory[0xff08]=0x00;
	memory[0xff09]=0xe0; // JMP E000 

		// Put characters into screen memory
	memory[0xff10]=0xa2; // LDX immediate
	memory[0xff11]=0x00; // X = 0
	memory[0xff12]=0xbd; // LDA absolute,X
	memory[0xff13]=0x00;
	memory[0xff14]=0xfe; // A = FE00,X
	memory[0xff15]=0x9d; // STA absolute,x
	memory[0xff16]=0x00;
	memory[0xff17]=0x10; // A = 1000,X (screen memory)
	memory[0xff18]=0xe8; // INX
	memory[0xff19]=0xe0; // CPX immediate
	memory[0xff1a]=0xa; // 20 characters
	memory[0xff1b]=0xd0; // BNE
	memory[0xff1c]=0xf7; // -9
	memory[0xff1d]=0x60; // RTS

	memory[0xfcfe]=0x60; // RTS - to Monitor
	memory[0xfffa]=0x00; // NMI Vector
	memory[0xfffb]=0xff; // Redraw screen and RTI
	memory[0xfffc]=0x04; // Cold reset vector
	memory[0xfffd]=0xff; // FF04 - Code to write message to screen
	memory[0xfffe]=0x00; // IRQ Vector - Same as NMI currently
	memory[0xffff]=0xff; // Redraw screen and RTI
	
	
// Zero Page Setup 		Location	Contents Reference
	memory[0x0000]=0x2f;  // C64 Shadow ROM bits, not needed, but who knows
	memory[0x0002]=0xbb;  //	0x02		Used for 
	memory[0x0003]=0xd2;  //
	memory[0x0004]=0xd2;
	memory[0x000a]=0xbc;  //
	memory[0x000b]=0xd2;  //
	memory[0x000c]=0x02;  //
	memory[0x000d]=0x02;  //
	memory[0x0010]=0x02;  //
	memory[0x0011]=0x02;  //
	memory[0x00a0]=0xaa;	 //	0xa0 or 10101010	Used for AND tests.
	memory[0x00fc]=0x01; // Used for ZP,X tests
	memory[0x00fd]=0x02; // Used for ZP,X tests
	memory[0x00fe]=0x03; // Used for ZP,X tests
	memory[0x00ff]=0x04; // Used for ZP,X tests

	// 1000-17CF - Screen memory 80x25 lines (2000) characters.
	// This is set up in the initialization routine.

	// BXXX range
	memory[0xb0a0]=0x60; // RTS from previous JSR
	memory[0xb0a0]=0xaa; // Value for AND
	memory[0xb200]=0xfe; // Low byte for indirect JMP
	memory[0xb201]=0xfc; // high byte for indirect JMP PC should read fcfe
		
	// CXXX Range to Test OP codes
	memory[0xc000]=0x20; // JSR - 3 bytes
	memory[0xc001]=0xa0; // Low byte
	memory[0xc002]=0xb0; // high btye: address b0a0	
	memory[0xc003]=0x60; // RTS - to Monitor

	memory[0xc004]=0x2d; // AND Absolute - 3 bytes
	memory[0xc005]=0xa1; // Low byte
	memory[0xc006]=0xb0; // high btye: address b0a1
	memory[0xc007]=0x60; // RTS - to Monitor
	
	memory[0xc008]=0x29; // AND Zero Page - 2 bytes
	memory[0xc009]=0xa0; // ZP address
	memory[0xc00a]=0x60; // RTS - to Monitor

	memory[0xc00b]=0xa9;  // LDA Immediate - 2 bytes
	memory[0xc00c]=0x7f;  // 04 decimal into A
	memory[0xc00d]=0x60; // RTS - to Monitor

	memory[0xc00e]=0xa5; // LDA Zero Page - 2 bytes
	memory[0xc00f]=0x02; // ZP address containing bb
	memory[0xc010]=0x60; // RTS - to Monitor

	memory[0xc011]=0xb5; // LDA Zero Page,X - 2 bytes
	memory[0xc012]=0xfc; // ZP address
	memory[0xc013]=0x60; // RTS - to Monitor
	
	memory[0xc014]=0xbd;	 // LDA absolute,X - 3 bytes
	memory[0xc015]=0x00;
	memory[0xc016]=0xd0;	// 0xd000
	memory[0xc017]=0x60; // RTS - to Monitor

	memory[0xc018]=0xb9;	 // LDA absolute,Y - 3 bytes
	memory[0xc019]=0x01;
	memory[0xc01a]=0xd0;	// 0xd001		
	memory[0xc01b]=0x60; // RTS - to Monitor

	memory[0xc01c]=0xa1;	// LDA indirect,x  2 - bytes
	memory[0xc01d]=0x02;	// ZP base address for index
	memory[0xc01e]=0x60; // RTS - to Monitor

	memory[0xc01f]=0xb1;	// LDA indirect,y  2 - bytes
	memory[0xc020]=0x0a;	// ZP base address for index
	memory[0xc021]=0x60; // RTS - to Monitor

	memory[0xc022]=0xe8;	// INX
	memory[0xc023]=0x60; // RTS - to Monitor

	memory[0xc024]=0xc8;	// INY
	memory[0xc025]=0x60; // RTS - to Monitor

	memory[0xc026]=0x0a;	// ASL A
	memory[0xc027]=0x60;	// RTS - To Monitor
	
	memory[0xc10a]=0x6c; // Indirect JMP - 3 bytes
	memory[0xc10b]=0x00; // Low byte
	memory[0xc10c]=0xb2; // High byte
	memory[0xc10d]=0x60; // RTS - to Monitor

	// C2XX Multiple instruction tests
	memory[0xc201]=0xa9;  // LDA Immediate - 2 bytes
	memory[0xc202]=0x7f;  // 7f decimal into A
//	memory[0xc203]=0xe8;	// INX
	memory[0xc203]=0x69;	// ADC Immediate
	memory[0xc204]=0x1;		// Immediate value to add
	memory[0xc205]=0x60;	// RTS

	// Compare Instructions
	memory[0xc20a]=0xa9;	// LDA Immediate
	memory[0xc20b]=0x80;	// 00 into A
	memory[0xc20c]=0xc9;	// CMP Immediate
	memory[0xc20d]=0x00;	// 00 should pass
	memory[0xc20e]=0x60;	// RTS

	// Roll Instructions
	memory[0xc20f]=0x18;	// CLC
	memory[0xc210]=0xa9;	// LDA Immediate
	memory[0xc211]=0xaa;	// aa into A
	memory[0xc212]=0x6a;	// ROR
	memory[0xc213]=0x10;	// BPL
	memory[0xc214]=0xFF;	// Jump back to c212
	memory[0xc215]=0x60;	// RTS

	// DXXX Range for Absolute address fetches
	memory[0xd000]=0xda;
	memory[0xd001]=0xdb;
	memory[0xd002]=0xdc;
	memory[0xd003]=0xdd;
	memory[0xd2bb]=0x99;
	memory[0xd2bc]=0x88;
	memory[0xd2bd]=0x70;
	memory[0xd2be]=0x71;
	memory[0xd2d2]=0x44;
} // End BOARD_MEM_CONFIG
