#include "fsm.h"

void BOARD_MEM_CONFIG(){
	
	// Setup Memory for testing

		// Zero Page config
		memory[0x0000]=0x00;
		memory[0x0001]=0x00;
		memory[0x0002]=0x00;	// 32 bit register A	Hi
		memory[0x0003]=0x00;	// 32 bit register A
		memory[0x0004]=0x00;	// 32 bit register A
		memory[0x0005]=0xee;	// 32 bit register A	Lo
		memory[0x0006]=0x00;	// 32 bit register B	Hi
		memory[0x0007]=0x00;	// 32 bit register B
		memory[0x0008]=0x01;	// 32 bit register B
		memory[0x0009]=0x90;	// 32 bit register B	Lo
		memory[0x000a]=0x00;
		memory[0x000b]=0x00;
		memory[0x000c]=0x00;
		memory[0x000d]=0x00;
		memory[0x000e]=0x00;
		memory[0x000f]=0x00;

		
		// Infinite loop at $500
		memory[0x0500]=0x4c; // jmp $500
		memory[0x0501]=0x00;
		memory[0x0502]=0x05;
	
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

		/* Maths Functions
		 * 
		 * ADD 2 32 bit values, RegA=RegA+RegB
		 * 	$fe0a   a6 04           LDX #$04
			$fe0c   18              CLC
			$fe0d   b5 01           LDA $01,X
			$fe0f   75 05           ADC $05,X
			$fe11   95 01           STA $01,X
			$fe13   ca              DEX
			$fe14   d0 f9           BNE $f9
			$fe16   60              RTS
		*/
	memory[0xfe0a]=0xa2;
	memory[0xfe0b]=0x04;
	memory[0xfe0c]=0x18;
	memory[0xfe0d]=0xb5;
	memory[0xfe0e]=0x01;
	memory[0xfe0f]=0x75;
	memory[0xfe10]=0x05;
	memory[0xfe11]=0x95;
	memory[0xfe12]=0x01;
	memory[0xfe13]=0xca;
	memory[0xfe14]=0xd0;
	memory[0xfe15]=0xf9;
	memory[0xfe16]=0x60;

	/*	Subtract 2 32bit numbers RegA=RegA-RegB
		$Fe1a   a2 04           LDX #$04
		$fe1c   38              SEC
		$fe1d   b5 01           LDA $01,X
		$fe1f   f5 05           SBC $05,X
		$fe21   95 01           STA $01,X
		$fe23   ca              DEX
		$fe24   d0 f9           BNE $f9
		$fe26   60              RTS
	*/
	memory[0xfe1a]=0xa2;
	memory[0xfe1b]=0x04;
	memory[0xfe1c]=0x38;
	memory[0xfe1d]=0xb5;
	memory[0xfe1e]=0x01;
	memory[0xfe1f]=0xf5;
	memory[0xfe20]=0x05;
	memory[0xfe21]=0x95;
	memory[0xfe22]=0x01;
	memory[0xfe23]=0xca;
	memory[0xfe24]=0xd0;
	memory[0xfe25]=0xf9;
	memory[0xfe26]=0x60;


	memory[0xff00]=0x20; // JSR to initial screen output routine
	memory[0xff01]=0x10; // Lo byte
	memory[0xff02]=0xff; // hi byte
	memory[0xff03]=0x40; // RTI for BRK vector
	
		// Code to jump to Start of main program
	memory[0xff04]=0x20; // JSR to screen print
	memory[0xff05]=0x10;
	memory[0xff06]=0xff;
	memory[0xff07]=0x60; // RTS
	memory[0xff08]=0x00;
	memory[0xff09]=0xe0; // JMP E000 

		/* Put characters into screen memory
		 * 	$ff10   a2 00           LDX #$00
			$ff12   bd 00 fe        LDA $fe00,X
			$ff15   8d 00 10        STA $1000
			$ff18   a9 a0           LDA #$a0
			$ff1a   8d 01 10        STA $1001
			$ff1d   e8              INX
			$ff1e   e0 0a           CPX #$0a
			$ff20   d0 f2           BNE $f2		FF=-1, so F2=-14 bytes
			$ff22   60              RTS
			*/
	memory[0xff10]=0xa2;	// Load X with 0
	memory[0xff11]=0x00;
	memory[0xff12]=0xbd;	// Load A with contents of $fe00,inexed by x
	memory[0xff13]=0x00;
	memory[0xff14]=0xfe;
	memory[0xff15]=0x8d;	// Store A into E_LCD_DATA
	memory[0xff16]=0x00;
	memory[0xff17]=0x10;
	memory[0xff18]=0xa9;	// Load A with a0 - E_LCD_DO_SOMETHING | E_LCD_PRINT_CHAR
	memory[0xff19]=0xa0;
	memory[0xff1a]=0x8d;	// Store A into E_LCD_REG
	memory[0xff1b]=0x01;
	memory[0xff1c]=0x10;
	memory[0xff1d]=0xe8;
	memory[0xff1e]=0xe0;	// Compare X with decimal 10 (characters to print)
	memory[0xff1f]=0x0a;
	memory[0xff20]=0xd0;	// If not, jump back to $ff12 to repeat
	memory[0xff21]=0xf2;
	memory[0xff22]=0x60;	// Return to calling routine

	memory[0xfcfe]=0x60; // RTS - to Monitor
	memory[0xfffa]=0x00; // NMI Vector
	memory[0xfffb]=0xff; // Redraw screen and RTI
	memory[0xfffc]=0x04; // Cold reset vector
	memory[0xfffd]=0xff; // FF04 - Code to write message to screen
	memory[0xfffe]=0x00; // IRQ Vector - Same as NMI currently
	memory[0xffff]=0xff; // Redraw screen and RTI
	
	

} // End BOARD_MEM_CONFIG
