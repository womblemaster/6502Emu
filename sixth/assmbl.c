#include <stdlib.h>
#include <ctype.h>
#include "fsm.h"

#define IMM 0
#define ABS 1
#define ABX 2
#define ABY 3
#define ZP 4
#define ZPX 5
#define ZPY 6
#define IDX 7
#define IDY 8
#define IND 9
#define ACC 10

char MNEM[][4]=
		{"ADC\0","AND\0","ASL\0","BCC\0","BCS\0","BEQ\0","BIT\0","BMI\0","BNE\0","BPL\0","BRK\0","BVC\0","BVS\0","CLC\0",
		"CLD\0","CLI\0","CLV\0","CMP\0","CPX\0","CPY\0","DEC\0","DEX\0","DEY\0","EOR\0","INC\0","INX\0","INY\0","JMP\0","JSR\0",
		"LDA\0","LDX\0","LDY\0","LSR\0","NOP\0","ORA\0","PHA\0","PHP\0","PLA\0","PLP\0","ROL\0","ROR\0","RTI\0","RTS\0","SBC\0",
		"SEC\0","SED\0","SEI\0","STA\0","STX\0","STY\0","TAX\0","TAY\0","TSX\0","TXA\0","TXS\0","TYA\0"};





//	In the following addressing table modes, branches are interpreted
//	as ZP, as they mnemonically look the same, so for our purposes it
//	doesn't matter, as long as the OP code is correct.
//	 Imm  ABS  AB,X AB,Y ZP   ZP,X ZP,Y ID,X ID,Y Ind  Acc/Imp
char MNOP[]={
     0x69,0x6D,0x7D,0x79,0x65,0x75,0xFF,0x61,0x71,0xFF,0xFF,	// ADC
     0x29,0x2D,0x3D,0x39,0x25,0x35,0xFF,0x21,0x31,0xFF,0xFF,	// AND
     0xFF,0x0E,0x1E,0xFF,0x06,0x16,0xFF,0xFF,0xFF,0xFF,0x0A,	// ASL
     0xFF,0xFF,0xFF,0xFF,0x90,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BCC
     0xFF,0xFF,0xFF,0xFF,0xB0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BCS
     0xFF,0xFF,0xFF,0xFF,0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BEQ
     0xFF,0x2C,0xFF,0xFF,0x24,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BIT
     0xFF,0xFF,0xFF,0xFF,0x30,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BMI
     0xFF,0xFF,0xFF,0xFF,0xD0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BNE
     0xFF,0xFF,0xFF,0xFF,0x10,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BPL
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,	// BRK
     0xFF,0xFF,0xFF,0xFF,0x50,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BVC
     0xFF,0xFF,0xFF,0xFF,0x70,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// BVS
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x18,	// CLC
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xD8,	// CLD
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x58,	// CLI
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xB8,	// CLV
     0xC9,0xCD,0xDD,0xD9,0xC5,0xD5,0xFF,0xC1,0xD1,0xFF,0xFF,	// CMP
     0xE0,0xEC,0xFF,0xFF,0xE4,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// CPX
     0xC0,0xCC,0xFF,0xFF,0xC4,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// CPY
     0xFF,0xCE,0xDE,0xFF,0xC6,0xD6,0xFF,0xFF,0xFF,0xFF,0xFF,	// DEC
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCA,	// DEX
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x88,	// DEY
     0x49,0x4D,0x5D,0x59,0x45,0x55,0xFF,0x41,0x51,0xFF,0xFF,	// EOR
     0xFF,0xEE,0xFE,0xFF,0xE6,0xF6,0xFF,0xFF,0xFF,0xFF,0xFF,	// INC
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE8,	// INX
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC8,	// INY
     0xFF,0x4C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x6C,0xFF,	// JMP
     0xFF,0x20,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	// JSR
     0xA9,0xAD,0xBD,0xB9,0xA5,0xB5,0xFF,0xA1,0xB1,0xFF,0xFF,	// LDA
     0xA2,0xAE,0xFF,0xBE,0xA6,0xFF,0xB6,0xFF,0xFF,0xFF,0xFF,	// LDX
     0xA0,0xAC,0xBC,0xFF,0xA4,0xB4,0xFF,0xFF,0xFF,0xFF,0xFF,	// LDY
     0xFF,0x4E,0x5E,0xFF,0x46,0x56,0xFF,0xFF,0xFF,0xFF,0x4A,	// LSR
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xEA,	// NOP
     0x09,0x0D,0x1D,0x19,0x05,0x15,0xFF,0x01,0x11,0xFF,0xFF,	// ORA
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x48,	// PHA
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x08,	// PHP
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x68,	// PLA
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x28,	// PLP
     0xFF,0x2E,0x3E,0xFF,0x26,0x36,0xFF,0xFF,0xFF,0xFF,0x2A,	// ROL
     0xFF,0x6E,0x7E,0xFF,0x66,0x76,0xFF,0xFF,0xFF,0xFF,0x6A,	// ROR
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x40,	// RTI
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x60,	// RTS
     0xE9,0xED,0xFD,0xF9,0xE5,0xF5,0xFF,0xE1,0xF1,0xFF,0xFF,	// SBC
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x38,	// SEC
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,	// SED
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x78,	// SEI
     0xFF,0x8D,0x9D,0x99,0x85,0x95,0xFF,0x81,0x91,0xFF,0xFF,	// STA
     0xFF,0x8E,0xFF,0xFF,0x86,0xFF,0x96,0xFF,0xFF,0xFF,0xFF,	// STX
     0xFF,0x8C,0xFF,0xFF,0x84,0x94,0xFF,0xFF,0xFF,0xFF,0xFF,	// STY
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xAA,	// TAX
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xA8,	// TAY
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBA,	// TSX
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x8A,	// TXA
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x9A,	// TXS
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x98};	// TYA



static char multiflag=0;

static short int getmnemonic(char *mnemonic,int addmode){
	short int value; // used for returning the enumeration value
	int index=256,j;
	// lookup mnemonic from MNEM enumeration table
	for (j=0;j<56;j++)
		if(strcmp(mnemonic,MNEM[j])==0){
			index=j;break;
		}

	// if index==256 we didn't find anything so error
	if (index<256) {
		return(MNOP[(index*11)+addmode]);
	}
	else{ return 255;} // error condition
}

static char* checkflag(int *flag,char *endp){
	// set flags based on next character

	if(*endp=='('){ // if open bracket then
		(*flag|=32+128); // set paren and indirect flag; it might not be indirect, reset on comma
		endp++;
	}else if(*endp==')'){
		(*flag&=~32); // clear paren flag
		endp++;
	}

	switch(*endp){
		case '\0':	// check for end of string
		case '\n': 	if(!multiflag)*flag|=1; break; // if multi-byte flag set, no nothing

		case '#': *flag|=2; endp++;  // no break as we want fall through

		case '$': *flag|=4; endp++; break;

		case '%': *flag|=8; endp++; break;

		case ',': 	// if , and indirect is set then likely to be indirect indexed or indexed indirect
					if((*flag&160)==160) *flag|=256; // must be indexed indirect notation (aa,x)
					if((*flag&160)==128) *flag|=512; // must be indirect indexed notation (aa),y
					*flag|=64;	// set comma flag, as indexing might be absolute or zero page
					endp++; break;

		default : *flag|=16; break;
	}
	multiflag=1; // gone through once, so set multibyte flag
	return endp; // successful condition
} // End of checkflag


static ushort ASSMBL(ushort location,char *text){
	char *operand=text;
	char *endp=text;
	char temp[50],*copy=temp,*mnemonic=temp;
	int operandlen,result,operandvalue;
	unsigned char hibyte, lobyte, opcode,index;
	int j=0,   flag; // flag= 	X X X X X X X X X X X X X X X
			//                			                    ^- 1. Single Byte OP code
			//        				   	                  ^--- 2. Immediate instruction # exists
			//	      				                    ^----- 4. Hexadecimal Operand $ exists
			//          			                  ^------- 8. Binary Operand % exists
			//        			                    ^--------- 16. Decimal Operand no prefix esitst
			//					                  ^----------- 32. Peranthesis exists, this flags on (
			//					                ^------------- 64. Comma; an index exists
			//					              ^--------------- 128.Indirect
			//					            ^----------------- 256. Indexed indirect (MEM,X)
			//					          ^------------------- 512. Indirect Indexed (MEM),Y
			//						    ^--------------------- 1024. Y Register
			//					      ^----------------------- 2048. X Register
			//					    ^------------------------- 4096. Error Condition
			//				      ^--------------------------- 8192. 2 byte instruction
			//				    ^----------------------------- 16384. 3 byte instruction

	// Clear flag
	flag=0;

	while (*endp==' ') endp++; // ignore white spaces in source
	// get mnemonic
	while(*endp!=' '&&*endp!='\0'&&*endp!='\n'){
		*copy++=toupper(*endp++);  // make sure mnemonic is in upper case for lookup later
	} // End while for get mnemonic

	*copy++='\0'; // terminate mnemonic with null
	operand=copy; // point operand at start of operand field in copy string

	while (*endp==' ') endp++; // ignore white spaces in source between Op and Operand

	// Check and set flags accordingly FIRST, (#$% or single byte instruction
	endp=checkflag(&flag,endp);

	// get operand
	while(*endp!=','&&*endp!='\0'&&*endp!='\n'&&*endp!=')'){ // next delimiter
		*copy++=*endp++;
	}
	operandlen=copy-operand; // how long is the operand?

	*copy++='\0'; // terminate operand with null

	// Check and set flags accordingly SECOND, ),
	endp=checkflag(&flag,endp);

	// Convert operand stirng to int
	if(flag&4) operandvalue=(int)strtol(operand,NULL,16);
	if(flag&8) operandvalue=(int)strtol(operand,NULL,2);
	if(flag&16) operandvalue=(int)strtol(operand,NULL,10);

	// Clear hi and lo bytes as they might not be used
	lobyte=0;
	hibyte=0;

	// Get Hi and Lo bytes for Operand values
	if (operandvalue<256) lobyte=operandvalue;
	else {lobyte=operandvalue&255; hibyte=(operandvalue>>8)&255;}

	// Set X and Y flags
	if(toupper(*endp)=='X')flag|=2048;
	if(toupper(*endp)=='Y')flag|=1024;
	// Clear Error flag
	flag&=~4096;
	
	// set index register to invalid value
	index=255;

	// Check and set the addressing mode; syntax analysis

	// single byte then addressing move is Implied or Accumulator, set the array so that
	// both modes are on the same column
	if(flag&1)
	{
		if ((opcode=getmnemonic(mnemonic,ACC))==255)
			flag|=4096; // Error
	}
	else

	// immediate mode is checked against flag and indirect,comma not set
	// error condition is operand is >255
	if(flag&2 && ~flag&128 && ~flag&64 && operandvalue<256)
	{
		flag|=8192; // 2 byte instruction
		if ((opcode=getmnemonic(mnemonic,IMM))==255)
			flag|=4096; // Error
	}
	else
	
	// Zero Page, if flag immediate mode not set and indirect not set and operandvalue <255 and comma not set
	// Relative is the difficult one as only applies to branch, but same format at Zero page, so covered here
	if(~flag&128 && ~flag&2 && ~flag&64 && ~flag&128 && operandvalue<256)
	{
		flag|=8192; // 2 byte instruction
		if ((opcode=getmnemonic(mnemonic,ZP))==255)
			flag|=4096; // Error
	}
	else
	
	// Zero Page with index, if flag immediate mode not set and indirect not set and operandvalue <255 and comma set
	if(~flag&128 && ~flag&2 && flag&64 && operandvalue<256)
	{
		flag|=8192; // 2 byte instruction
		if(flag&2048) index=ZPX; // is index register X
		if(flag&1024) index=ZPY; // is index register Y
		if (index!=255) 
			if ((opcode=getmnemonic(mnemonic,index))==255)
				flag|=4096; // Error
	}
	else

	
	// Absolute, operandvalue >255 <65536 and (comma) not set
	if(~flag&128 && ~flag&2 && ~flag&64 && operandvalue>255 && operandvalue<65536)
	{
		flag|=16384; // 3 byte instruction
		if ((opcode=getmnemonic(mnemonic,ABS))==255)
			flag|=4096; // Error
	}
	else
	
	
	// Absolute indexed, operandvalue >255 <65536 with comma set and indirect not set
	if(~flag&128 && ~flag&2 && flag&64 && operandvalue>255 && operandvalue<65536)
	{
		flag|=16384; // 3 byte instruction
		flag|=4096; // Set the error flag, clear if instruction is successful
		if(flag&2048) index=ABX; // is index register X
		if(flag&1024) index=ABY; // is index register Y
		if (index!=255) 
			if ((opcode=getmnemonic(mnemonic,index))<255)
				flag&=~4096; // Clear Error Flag
	}
	else


	// indexed indirect, indexed indirect set and operandvalue <256   (MEM,X)
	if(flag&256 && ~flag&2 && flag&64 && operandvalue<256)
	{
		flag|=8192; // 2 byte instruction
		if(flag&2048)
			if ((opcode=getmnemonic(mnemonic,IDX))==255)
				flag|=4096; // Error
	}
	else


	// Indirect indexed, indirect indexed set and operandvalue <256    (MEM),Y
	if(flag&512 && ~flag&2 && flag&64 && operandvalue<256)
	{
		flag|=8192; // 2 byte instruction
		if(flag&4096)
			if ((opcode=getmnemonic(mnemonic,IDY))<255)
				flag|=4096; // Error
	}
	else
	
	
	// indirect instruction for jmp
	if(flag&128 && ~flag&64 && ~flag&2 && ~flag&1)
	{
		flag|=16384; // 3 byte instruction
		if ((opcode=getmnemonic(mnemonic,IND))==255)
			flag|=4096; // Error
	}


	else
		printf(" Syntax Error\n");

	if (flag&4096) printf("Bad Op Code\n");
	else if (flag&1) { // Single Byte
		memory[location++]=opcode;
	}
	else if (flag&8192) { // 2 Byte
		if(location>65534)printf("Memory violation\n");
		else{
			memory[location++]=opcode;
			memory[location++]=lobyte;
		}
	}
	else if (flag&16384) { // 3 Byte
		if(location>65533)printf("Memory violation\n");
		else{
			memory[location++]=opcode;
			memory[location++]=lobyte;
			memory[location++]=hibyte;
		}
	}
		
	// Check location is good
	if (location>65535)location-=65535;	// if our new location is > memory limit, then wrap back to 0
	// End
	return location; // return the next memory to assemble to
}





void ASSEMBLER(char ** tokens,int j){
	ushort ci, charcount,quit=0,base;
	size_t bufsize = 30;
	char *buffer;

    if((buffer = (char *)malloc(bufsize * sizeof(char))) == NULL)
    {
        perror("Run out of space for our buffer..!");
        quit=1;
    }

	if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
	ci=(int)strtol(tokens[1],NULL,base);  // convert string to integer

	if (ci>=0&&ci<65536){ // value is valid so let's begin
		
		while(!quit){
			printf("\t%x ",ci);	// our prompt
			charcount = getline(&buffer,&bufsize,stdin); // get input
			if (*buffer=='\n') quit=1;
			else ci=ASSMBL(ci,buffer);
		}
		
	} else printf("Bad Address\n");
	
}
