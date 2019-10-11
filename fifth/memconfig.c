#include "fsm.h"

void BOARD_MEM_CONFIG(){
	
	// Setup Memory for testing
	// ROM vectors first
	memory[0xff00]=0x40; // RTI for BRK vector
	memory[0xfcfe]=0x60; // RTS - to Monitor
	memory[0xfffc]=0x00;
	memory[0xfffd]=0xc0;
	memory[0xfffe]=0x00;
	memory[0xffff]=0xff;
	
	// Zero Page Setup 		Location	Contents Reference
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
	memory[0xc202]=0x7f;  // 04 decimal into A
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
	memory[0xc213]=0x60;	// RTS


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
