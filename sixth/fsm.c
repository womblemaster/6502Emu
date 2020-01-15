/*
 * Tidied up the SYS_RESET() routine
 * Changed the CPU registers to a structure
 * The Disassembler is now a better approach - assmbl.c
 * 
 */

#include "fsm.h"

// Allocate an Array to preten to be Memory of 64k
unsigned char memory[RAM];



// Working Variables
uchar CURRENT_INST;
uchar DONE;

int main(){
	int result,j=0,quit=0;
    char *inputbuffer,*tokens[20];
    size_t bufsize = 32;
    size_t charcount;

    if((inputbuffer = (char *)malloc(bufsize * sizeof(char))) == NULL)
    {
        perror("Run out of space for our buffer..!");
        return 1;
    }

	result=SYS_RESET();

	PRINT_REG();	// Display the current register states

	
// Main loop
	while(!quit){

		// Get input from user as to what they want to do.
		j=0;
		printf(": ");	// our prompt
		charcount = getline(&inputbuffer,&bufsize,stdin); // get input

		// Tokenize the string using stdlib.h strtok.
		// tokens is a pointer to array of pointers into inputbuffer
		// As strtok replaces the delimeter (space for us) with /0
		tokens[j] = strtok (inputbuffer," ");
		while ((tokens[j]!= NULL))
		{
			tokens[++j] = strtok (NULL, " ");
		}

		switch ((*tokens)[0]){

			case 'h':	// help
			case 'H':	help();
						break;
			case 'a':
			case 'A':	// Assembler
						if (j<2) printf("Usage:\t(a)ssemble <start address>\n");
						else ASSEMBLER(tokens,j);
						break;
						
			case 'd':	if (j<3) printf("Usage:\t(d)isassemble <start address> <end address>\n");
						else DISASSEMBLE(tokens, j);
						break;
			
			case 'f':	// Fill Memory
						if (j<4) printf("Usage:\t(f)ill <start address> <end address> <contents>\n");
						else FILL(tokens,j);
						break;
						
			case 'g':	EXECUTE(tokens,j);
						break;

			case 'i':	if (j<3) printf("Usage:\t(i)nsert <mem address> <value>\n");
						else Mon_InsertMem(tokens);
						break;

			case 'm':	if (j<2) printf("Usage:\t(m)emory <start addr> <end addr>\n\t(m)emory address\n\n");
						else Mon_DisplayMem(tokens,j);
						break;

			case 'r':	PRINT_REG();	// Display the current register states
						break;

			case 'q':	printf("Quitting...\n");
						quit=1;
						break;

			case 's':	EXECUTE(tokens,j);
						break;
					
			case 't':	if (j<2) printf("Usage:\t(t)oggle <flag letter>\n");
						else Mon_ToggleFlags(tokens);
						break;
			
			case 'x':	SYS_RESET();
			
			default: break;
		}
	}

	free(inputbuffer);
	result=CLEAN_UP();
	return 0;
}

int SYS_RESET(){
	int j;

	// Set up the CPU, this isn't strictly correct, as the registers
	// don't necessarily get set up.
	CPU.SR=32;
	CPU.SP=0xff;
	CPU.PC=0xfffc;
	CPU.A=0;
	CPU.X=0;
	CPU.Y=0;

	BOARD_MEM_CONFIG(); // Go and set up the memory

	for (j=0x1000;j<0x17d0;j++) memory[j]=0x20; // Set screen to contain spaces

// Load PC from ROM low byte first
	CPU.PC=memory[0xfffc];
	CPU.PC<<=8;
	CPU.PC|=memory[0xfffd];
	CPU.PC=(CPU.PC>>8)|(CPU.PC<<8);  // Swap bytes
	return 0;
}


int CLEAN_UP(){	printf("Clean UP\n");}

void INST_FETCH(){
	
	CURRENT_INST=memory[CPU.PC++];
	
}  // End INST_FETCH


void EXECUTE(char **tokens,int j){
	unsigned short int start;
	int base,result;
	char c;
	
	if (j < 2) start = CPU.PC;  // No arguments, then current program counter is the start
	else { // next parameter is start address to use
		if ((tokens)[1][0]=='#') base =10; else base=16;
		start=(unsigned short int)strtol(tokens[1],NULL,base);
	}  // End condition to select start

	CPU.PC=start;
	c=(tokens)[0][0];
	switch (c){
		case 'g':	// Run until last RTS
					DONE=0;
					while (!DONE){ // While something to do (No RTS)
						CURRENT_INST=memory[CPU.PC++];
						result=(*instfunc[CURRENT_INST])(CURRENT_INST);
						//KEYBOARD_HANDLER(); // Do something with Keyboard input
						//SCREEN_HANDLER();  // Do something with screen
					}
					break;
		case 's':	// Step a single instruction
					CURRENT_INST=memory[CPU.PC++];
					result=(*instfunc[CURRENT_INST])(CURRENT_INST);
					PRINT_REG();
					break;
		default: break;
	} // End While
}  // End EXECUTE


void FILL(char **tokens, int j){	
	unsigned short int k=0,result1,result2,base;
	uchar contents;
	
	if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
	result1=(int)strtol(tokens[1],NULL,base);  // convert string to integer
	result2=(int)strtol(tokens[2],NULL,base);  // convert string to integer
	contents=(int)strtol(tokens[3],NULL,base);  // convert string to integer

	for (k=result1;k<=result2;k++) memory[k]=contents; // set memory contents

} // End Fill


void PRINT_REG(){
	printf("\n A\t X\t Y\tSP\t PC\tN V _ B D I Z C\n%2x\t%2x\t%2x\t%2x\t%4x\t",CPU.A,CPU.X,CPU.Y,CPU.SP,CPU.PC);
	PRINT_BITS(sizeof(CPU.SR),&CPU.SR);
	printf("\n\n");
}  // End PRINT_REG

void Mon_InsertMem(char **tokens){
	int base,address,content;
	
	if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
	address=(int)strtol(tokens[1],NULL,base); // convert address parameter to integer
	content=(int)strtol(tokens[2],NULL,base);

	memory[address]=content;  // Set memory to new value

}  // End Mon_InsertMem


void Mon_DisplayMem(char **tokens, int j){
	unsigned short int i=0,k=0,result1,result2,base,line=0;
	
	if (j<3){  // one parameter or two?
		if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
		result1=(int)strtol(tokens[1],NULL,base);  // convert string to integer
		printf("\n\n Add\tHEX\tDEC\n0x%x:\t%x\t%d\n\n",result1,memory[result1],memory[result1]);		
	}else {
		if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
		result1=(int)strtol(tokens[1],NULL,base);  // convert string to integer
		result2=(int)strtol(tokens[2],NULL,base);  // convert string to integer
		k=result1;
		while(k<=result2){		
			printf("0x%x:\t",k); // Print base address
			line=0; i=k; // set up line counter and temp memory counter
			while((line<8)&&(k<=result2)){ // print 8 rows
				printf("%02x  ",memory[k++]); line++;
			}
			while (i<k) printf("%c",(memory[i++]<0x20)?('.'):(memory[i-1])); // print memory as characters
			printf("\n"); // new line at end of row
		}
	}
}  // End Mon_DisplayMem


void Mon_ToggleFlags(char **tokens){

	switch((tokens)[1][0]){
		case 'n': (CPU.SR)^=(uchar)128;
					break;
		case 'v': (CPU.SR)^=64;
					break;
		case 'b': (CPU.SR)^=16;
					break;
		case 'd': (CPU.SR)^=8;
					break;
		case 'i': (CPU.SR)^=4;
					break;
		case 'z': (CPU.SR)^=2;
					break;
		case 'c': (CPU.SR)^=1;
					break;
		default:	printf("\nUnknown Flag in Status Register\n\n");
					break;
	
	}
} // End Mon_SetFlags

void help(){

	printf("6510 cli help\n*************\nletters in () are shortcuts\n\n");
	printf("\nAddresses and values default is hex or if decimal required prefix with #.\nIf the first value is hex, the second is presumed to be hex\n\n");
	printf("(a)ssemble -\tAssemble one line at a time: a <start address>\n");
	printf("(d)isssembler -\tDisassemble memory displaying mnemonic: d <start address> <end address>\n");
	printf("(f)ill -\tFill Memory: f <start address> <end address> <contents>\n");
	printf("(g)o -\t\tRun code: g on its own will start the code from current PC\n\t\tg <address> runs from a specific address\n");
	printf("(h)elp -\tthis help screen.  To get help on the command, type the command and <enter>\n");
	printf("(i)nsert-\tInsert into Memory, i <address> <value>\n");
	printf("(m)emory -\tMemory dump, can be run as either: m <address>\n\t\tor: m <start address> <end address>\n");
	printf("(r)egisters -\tDisplay the CPU registers\n");
	printf("(s)tep -\tSingle Step instruction\n");
	printf("(t)oggle -\tToggle a SR flag: t <flag letter>\n");	
	printf("(q)uit -\texits program back to shell\n");
	printf("(x) -\te(x)terminate - System Reset\n");

}  // End Help

void PRINT_BITS(size_t const size, void const * const ptr){

	// Nice routine to print any length and data type in binary
	// https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
	//
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u ", byte);
        }
    }
    puts("");
}  // End PRINT_BITS
