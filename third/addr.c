/*
 * The file handles all of the 6502 addressing modes in separate routines
 * 
 * The idea here is that any addressing mode is only really a way of
 * returning some content of memory, so this allows us to create reusable
 * code that is easily tested.
 */

#include "fsm.h"

/*
 * Return the 8bit Contents of the memory pointed to by PC
 * increment PC ready for next instruction
 */
uchar AM_IMMEDIATE(){
	return memory[CPU_PC++];
}  // End AM_IMMEDIATE

/*
 * Return the 8bit contents of memory on Zero Page
 * Pointed to by the 8bit memory location currently pointed
 * to by the PC!!!
 */
uchar AM_ZERO_PAGE(){
	return memory[(memory[CPU_PC++])];
}  // End AM_ZERO_PAGE

/*
 * Return the 8bit contents of memory anywhere
 * Pointed to by the 16bit value located in PC and PC+1
 * remember little endian, so low byte first.
 */
uchar AM_ABSOLUTE(){
	unsigned short int Temp;
	Temp=memory[CPU_PC++];
	Temp|=(memory[CPU_PC++])<<8;
	return memory[Temp];
}  // End AM_ABSOLUTE

uchar AM_IDX_ABS(){}
uchar AM_IDX_ZP(){}
uchar AM_IND_X(){}
uchar AM_IND_Y(){}
