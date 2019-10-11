/* 	implied (1-byte instructions);
	absolute (3 bytes);
	indexed absolute (3 bytes);
	indexed zero-page (2 bytes);
	relative (2 bytes);
	accumulator (1);
	indirect,x and indirect,y (2); 
	immediate (2).
	Absolute mode is a general-purpose mode.
	Branch instructions use a signed 8-bit offset relative to the instruction after the branch;
	the numerical range −128..127 therefore translates to 128 bytes backward and 127 bytes forward from the 
	instruction following the branch (which is 126 bytes backward and 129 bytes forward from the start of the 
	branch instruction). Accumulator mode uses the accumulator as an effective address and does not need any operand data. 
	Immediate mode uses an 8-bit literal operand
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define RAM 65536 // Amount of RAM addressable with 16 bits of program counter
#define CPU_CLOCK 1000000 // Clock Speed of CPU in Hz

// who can be bothered typing unsigned char all the time
typedef unsigned char uchar;

// Allocate an Array to pretent to be Memory of 64k
extern unsigned char memory[RAM];

// 6510 Resisters
extern uchar CPU_SP;
extern unsigned short int  CPU_PC; 
extern uchar CPU_A;
extern uchar CPU_X;
extern uchar CPU_Y;
extern uchar CPU_SR; // SV_BDIZC bit 5 is unused and always 1
extern int _CPU_CLOCK;

// Working Variables
extern uchar CURRENT_INST;
extern uchar DONE;

// declare all system functions
int SYS_RESET();  // Setup the environment, registers etc
int CLEAN_UP();		// do any/all cleanup required
void PRINT_REG();	// Display registers
void PRINT_BITS();
void Mon_InsertMem(char **);
void Mon_DisplayMem(char**,int);
void Mon_ToggleFlags(char **);
void help();
void INST_FETCH();
void EXECUTE(char **,int);
void BOARD_MEM_CONFIG();

// Addressing Functions; not sure if I need these, but will place holder them
uchar AM_IMMEDIATE();
uchar AM_ZERO_PAGE();
uchar AM_ABSOLUTE();
uchar AM_ABS_X();
uchar AM_ABS_Y();
uchar AM_ZP_X();
uchar AM_ZP_Y();
uchar AM_IND_X();
uchar AM_IND_Y();


// Forward lookup to mnemonic function handlers
int I_BRK(uchar);	// CPU BRK instruction
int I_ORA(uchar);	// CPU Logical OR on Accumulator
int I_UDEF(uchar);  // Deals with all illegal OP Codes
int I_ASL(uchar);
int I_STC(uchar);  // Stack Instructions: TXS,TSX,PHA,PLA,PHP,PLP
int I_BCH(uchar);  // Handles all Branches and variations
int I_FLG(uchar);  // Deals with CPU SR Flags
int I_JSR(uchar);
int I_AND(uchar);
int I_BIT(uchar);
int I_ROL(uchar);
int I_RTI(uchar);
int I_EOR(uchar);
int I_LSR(uchar);
int I_JMP(uchar);
int I_RTS(uchar);
int I_ADC(uchar);
int I_ROR(uchar);
int I_BVS(uchar);
int I_ROR(uchar);
int I_STA(uchar);
int I_STY(uchar);
int I_STX(uchar);
int I_LDA(uchar);
int I_LDY(uchar);
int I_LDX(uchar);
int I_CPY(uchar);
int I_CMP(uchar);
int I_CPX(uchar);
int I_SBC(uchar);
int I_DEC(uchar);
int I_INC(uchar);
int I_NOP(uchar);  // CPU No Operation instruction
int I_REG(uchar);  // Implied Register instructions, TAX,TXA,DEX,INX,TAY,TYA,DEY,INY
int I_INC(uchar);

// Pointers to functions, in range 00-FF, each pointer maps directly to the OP code hex value
static 
int (*instfunc[])(uchar)={	I_BRK , I_ORA , I_UDEF , I_UDEF , I_UDEF , I_ORA , I_ASL , I_UDEF , I_STC , I_ORA , I_ASL , I_UDEF , I_UDEF , I_ORA , I_ASL , I_UDEF , 
							I_BCH , I_ORA , I_UDEF , I_UDEF , I_UDEF , I_ORA , I_ASL , I_UDEF , I_FLG , I_ORA , I_UDEF , I_UDEF , I_UDEF , I_ORA , I_ASL , I_UDEF , 
							I_JSR , I_AND , I_UDEF , I_UDEF , I_BIT , I_AND , I_ROL , I_UDEF , I_STC , I_AND , I_ROL , I_UDEF , I_BIT , I_AND , I_ROL , I_UDEF , 
							I_BCH , I_AND , I_UDEF , I_UDEF , I_UDEF , I_AND , I_ROL , I_UDEF , I_FLG , I_AND , I_UDEF , I_UDEF , I_UDEF , I_AND , I_ROL , I_UDEF , 
							I_RTI , I_EOR , I_UDEF , I_UDEF , I_UDEF , I_EOR , I_LSR , I_UDEF , I_STC , I_EOR , I_LSR , I_UDEF , I_JMP , I_EOR , I_LSR , I_UDEF ,
							I_BCH , I_EOR , I_UDEF , I_UDEF , I_UDEF , I_EOR , I_LSR , I_UDEF , I_FLG , I_EOR , I_UDEF , I_UDEF , I_UDEF , I_EOR , I_LSR , I_UDEF , 
							I_RTS , I_ADC , I_UDEF , I_UDEF , I_UDEF , I_ADC , I_ROR , I_UDEF , I_STC , I_ADC , I_ROR , I_UDEF , I_JMP , I_ADC , I_ROR , I_UDEF , 
							I_BCH , I_ADC , I_UDEF , I_UDEF , I_UDEF , I_ADC , I_ROR , I_UDEF , I_FLG , I_ADC , I_UDEF , I_UDEF , I_UDEF , I_ADC , I_ROR , I_UDEF , 
							I_UDEF , I_STA , I_UDEF , I_UDEF , I_STY , I_STA , I_STX , I_UDEF , I_REG , I_UDEF , I_REG , I_UDEF , I_STY , I_STA , I_STX , I_UDEF , 
							I_BCH , I_STA , I_UDEF , I_UDEF , I_STY , I_STA , I_STX , I_UDEF , I_REG , I_STA , I_STC , I_UDEF , I_UDEF , I_STA , I_UDEF , I_UDEF , 
							I_LDY , I_LDA , I_LDX , I_UDEF , I_LDY , I_LDA , I_LDX , I_UDEF , I_REG , I_LDA , I_REG , I_UDEF , I_LDY , I_LDA , I_LDX , I_UDEF , 
							I_BCH , I_LDA , I_UDEF , I_UDEF , I_LDY , I_LDA , I_LDX , I_UDEF , I_FLG , I_LDA , I_STC , I_UDEF , I_LDY , I_LDA , I_LDX , I_UDEF , 
							I_CPY , I_CMP , I_UDEF , I_UDEF , I_CPY , I_CMP , I_DEC , I_UDEF , I_REG , I_CMP , I_REG , I_UDEF , I_CPY , I_CMP , I_DEC , I_UDEF , 
							I_BCH , I_CMP , I_UDEF , I_UDEF , I_UDEF , I_CMP , I_DEC , I_UDEF , I_FLG , I_CMP , I_UDEF , I_UDEF , I_UDEF , I_CMP , I_DEC , I_UDEF , 
							I_CPX , I_SBC , I_UDEF , I_UDEF , I_CPX , I_SBC , I_INC , I_UDEF , I_REG , I_SBC , I_NOP , I_UDEF , I_CPX , I_SBC , I_INC , I_UDEF , 
							I_BCH , I_SBC , I_UDEF , I_UDEF , I_UDEF , I_SBC , I_INC , I_UDEF , I_FLG , I_SBC , I_UDEF , I_UDEF , I_UDEF , I_SBC , I_INC , I_UDEF
};













