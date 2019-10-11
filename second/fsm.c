/*
 * Split the project into 2 files: fsm.c and inst.c 
 * 
 * The program now has a shell with some commands to allow us to input
 * OP codes and read memory.
 * 
 * main() - now has a loop that gets input, acts on the input and continues
 * 			until quit is requrested by the user.
 * 
 * There are then various routines to action the requested function
 * from the user.  This is flexible and will allow more commands to be
 * added as and when we need them.
 * 
 * We currently support:
 * Insert, Memory dumping, register display, help command annd Quit.
 * 
 * Only the first letter of each command is needed.
 * 
 * The tokenizer is built using strtok, basic but works for this.
 * 
 * All the instructions are now in a seperate file instc.
 * 
 **/


#include "fsm.h"


// Allocate an Array to pretent to be Memory of 64k
unsigned char memory[RAM];

// 6510 Resisters
uchar _SP=0xff;
unsigned short int _PC=0xfffc; 
uchar _A=0;
uchar _X=0;
uchar _Y=0;
uchar _SR=32; // SV_BDIZC bit 5 is unused and always 1


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
			case 'h':	help();
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
			
			default: break;
		}
	}


	result=CLEAN_UP();
	return 0;
}

int SYS_RESET(){

	FILE *fp;
	int basicmp=0xa000;
	int kernalmp=0xe000;
	// Set Address 0 to 0101111 (0x2F)
	memory[0]=0x2f;
	int j;

	memory[0xfffc]=0xaa;
	memory[0xfffd]=0xcc;

// Load PC from ROM low byte first
	_PC=memory[0xfffc];
	_PC<<=8;
	_PC|=memory[0xfffd];
	_PC=(_PC>>8)|(_PC<<8);  // Swap bytes
	return 0;
}

int CLEAN_UP(){	printf("Clean UP\n");}

void PRINT_REG(){
	printf("\nA\tX\tY\tSP\tPC\tS V _ B D I Z C\n%x\t%x\t%x\t%x\t%x\t",_A,_X,_Y,_SP,_PC);
	PRINT_BITS(sizeof(_SR),&_SR);
	printf("\n\n");
}  // End PRINT_REG

void Mon_InsertMem(char **tokens){
	int base,address,content;
	
	if ((tokens)[1][1]=='x') base =16; else base=10; // if parameter starts with 0x then base is 16 else base 10
	address=(int)strtol(tokens[1],NULL,base); // convert address parameter to integer
	content=(int)strtol(tokens[2],NULL,base);

	memory[address]=content;

}  // End Mon_InsertMem



void Mon_DisplayMem(char **tokens, int j){
	int k=0,result1,result2,base,line=0;
	
	if (j<3){  // one parameter or two?
		if ((tokens)[1][1]=='x') base =16; else base=10; // if parameter starts with 0x then base is 16 else base 10
		result1=(int)strtol(tokens[1],NULL,base);  // convert string to integer
		printf("\n\n Add\tHEX\tDEC\n0x%x:\t%x\t%d\n\n",result1,memory[result1],memory[result1]);		
	}else {
		if ((tokens)[1][1]=='x') base =16; else base=10; // if parameter starts with 0x then base is 16 else base 10
		result1=(int)strtol(tokens[1],NULL,base);  // convert string to integer
		result2=(int)strtol(tokens[2],NULL,base);  // convert string to integer
		for(k=result1;k<result2;k++)		
			printf("0x%x:\t%x\t%d\n",k,memory[k],memory[k]);
	}
}  // End Mon_DisplayMem


void help(){

	printf("6510 cli help\n*************\nletters in () are shortcuts\n\n");
	printf("\nAddresses and values can be in hex or decimal.\nIf the first value is hex, the second is presumed to be hex\n\n");
	printf("(h)elp -\tthis help screen.  To get help on the command, type the command and <enter>\n");
	printf("(i)nsert-\tInsert into Memory, i <address> <value>\n");
	printf("(m)emory -\tMemory dump, can be run as either: m <address>\n\t\tor: m <start address> <end address>\n");
	printf("(r)egisters -\tDisplay the CPU registers");
	printf("(q)uit -\texits program back to shell\n");

}  // End Help

void PRINT_BITS(size_t const size, void const * const ptr){

	// Nice routeine to print any length and data type in binary
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
