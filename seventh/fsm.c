/*
 * Major change: started to use NCurses libraries to handle input/output to terminal
 * Ncurses allows the terminal to be split into areas
 * This split means we can create a pseudo screen that we can map to memory, and the
 * emulator can use as output for the 6502 programs written
 * 
 * I have also added the CPU registers to their own window, so they are always displayed
 * The (r)egister command line is now gone, as well as the help output for it
 * 
 * Ncurses uses custom functions, so printf is changed to printw for main screen, or:
 * wprintw() for child windows.
 * 
 * There are also mvwprintw() functions to move the curser first, then print to a specific
 * area in a child window.
 * 
 * On Ubuntu, ncurses libraries have to be installed using
 * 
 * sudo apt-get install libncurses5-dev libncursesw5-dev
 * 
 * The disassembler now takes a single start address as well as start/end addresses
 * used in the step instruction to make debugging more understandable
 * 
 * Found and fixed some bugs with ADC(), SBC(), CMP(), CPX() and CPY().
 * This was down to me forgetting to check result for Zero on Reg-Mem
 * SBC, I had the borrow(carry) wrong.
 * 
 */

#include "fsm.h"

// static function declarations
static int disk(char *option,char **,int);

// Allocate an Array to preten to be Memory of 64k
unsigned char memory[RAM];

// Ncurses windows
WINDOW *lcd_win, *lcd_border_win, *reg_win, *main_win;

// Working Variables
uchar CURRENT_INST;
ushort B_POINT,BP_FLAG;


int main(){
	int result,j=0,quit=0;
    char *inputbuffer,*tokens[20];
    size_t bufsize = 32;
    size_t charcount;

	// We need some buffer space for inputting commands through the cli
    if((inputbuffer = (char *)malloc(bufsize * sizeof(char))) == NULL)
    {
        perror("Run out of space for our buffer..!");
        return 1;
    }

	result=SYS_RESET();  // Set up child windows, memory, registers and stuff

	if (LINES<32||COLS<80){ // Check Terminal is big enough
		endwin(); // Shutdown Curses
		printf("Terminal is not big enough\nMust be H32 x W80 minimum\n\nCurrent dimentions are Height=%d Width=%d\n\n",LINES,COLS);
		return 1; // Exit with error
	} // End Check Terminal size

// Main loop; keep going until Q or q is selected
	while(!quit){

		// Get input from user as to what they want to do.
		j=0;

		PRINT_REG();	// Display the current register states

		*inputbuffer='\0';
		wprintw(main_win,": ");	// our prompt
			// Use ncurses window version of get
		wgetstr(main_win,inputbuffer);

		if(*inputbuffer!='\0'){
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
	
				case ',':	// Load file
							disk("Load",tokens,j);
							break;
				case '.':	// Save file
							disk("Save",tokens,j);
							break;
	
				case 'a':
				case 'A':	// Assembler
							if (j<2) wprintw(main_win,"Usage:\t(a)ssemble <start address>\n");
							else ASSEMBLER(tokens,j);
							break;
				
				case 'b':	// Set Break Point
				case 'B':	SET_BREAK_POINT(tokens, j);
							break;
							
				case 'd':	if (j<2) wprintw(main_win,"Usage:\t(d)isassemble <start address> [end address]\n");
							else DISASSEMBLE(tokens, j);
							break;
				
				case 'f':	// Fill Memory
							if (j<4) wprintw(main_win,"Usage:\t(f)ill <start address> <end address> <contents>\n");
							else FILL(tokens,j);
							break;
							
				case 'g':	EXECUTE(tokens,j);
							break;
	
				case 'i':	if (j<3) wprintw(main_win,"Usage:\t(i)nsert <mem address> <value>\n");
							else Mon_InsertMem(tokens);
							break;
	
				case 'm':	if (j<2) wprintw(main_win,"Usage:\t(m)emory <start addr> <end addr>\n\t(m)emory address\n\n");
							else Mon_DisplayMem(tokens,j);
							break;
	
				case 'q':	wprintw(main_win,"Quitting...\n");
							quit=1;
							break;
	
				case 's':	EXECUTE(tokens,j);
							break;
						
				case 't':	if (j<2) wprintw(main_win,"Usage:\t(t)oggle <flag letter>\n");
							else Mon_ToggleFlags(tokens);
							break;
				
				case 'x':	SYS_RESET();
				
				default: break;
			} // End Switch/Case

		} // End if empty inputbuffer
		wrefresh(main_win); // need to refresh the window to display prompt
		refresh(); // refresh stdscr
	} // End While !Quit
	free(inputbuffer);
	return CLEAN_UP();
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

	// Clear Breakpoints
	B_POINT=0;
	BP_FLAG=0;



// Load PC from ROM low byte first
	CPU.PC=memory[0xfffc];
	CPU.PC<<=8;
	CPU.PC|=memory[0xfffd];
	CPU.PC=(CPU.PC>>8)|(CPU.PC<<8);  // Swap bytes

	// Initialise Ncurses
	initscr();	// ncurses main screen initialise
	cbreak();	// buffered input
	echo();		// Make sure terminal echo is enabled for stdscr
	refresh();	// Refresh the screen ready for use, this clears the terminal
	
	
	// Build NCurses windows
	lcd_border_win=newwin(LCD_BORDER_H, LCD_BORDER_W, 0, 0); // Height, Width, Starty, Startx
	lcd_win=newwin(LCD_HEIGHT, LCD_WIDTH, 1, 1); // Height, Width, Starty, Startx
	scrollok(lcd_win,TRUE);		// Enable scrolling 
	reg_win=newwin(LCD_BORDER_H, 42, 0, 24);	// Height, Width, Starty, Startx
									// Don't need scrolling in the reg window
	main_win=newwin(LINES-11, 80, 11, 0); // Height, Width, Starty, Startx
	scrollok(main_win,TRUE);
	
	// Put borders around the Register and output windows
	// I don't want one round the main window
	box(reg_win, STD_BORDER , STD_BORDER); // STD_BORDER is defined in fsm.h as 0
	box(lcd_border_win, STD_BORDER , STD_BORDER);


	// The register window only needs printing once now
	mvwprintw(reg_win, 1, 15, "6502 Registers");
	mvwprintw(reg_win, 3, 7, "Registers\t    SR Flags");
	mvwprintw(reg_win, 4, 2, "A   X   Y  SP  PC\tN V _ B D I Z C");
	wrefresh(lcd_border_win);
	wrefresh(lcd_win);
	wrefresh(reg_win);
	wrefresh(main_win);
	return 0;
}


int CLEAN_UP(){	
	endwin();
	printf("Clean Exit\n");
} // End CLEAN_UP


void SET_BREAK_POINT(char **tokens,int j){

	int base;
	
	if (j<2) {BP_FLAG=0; wprintw(main_win,"Break Point Cleared\n");wrefresh(main_win);}
	else
	{
		if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
		B_POINT=(int)strtol(tokens[1],NULL,base);  // convert string to integer
		BP_FLAG=1;
	}
}  // End  SET_BREAK_POINT

void INST_FETCH(){
	
	CURRENT_INST=memory[CPU.PC++];
	
}  // End INST_FETCH


void EXECUTE(char **tokens,int j){
	unsigned short int start;
	int base,result,DONE=0;
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
					result=0;
					keypad(lcd_win,TRUE);
					wtimeout(lcd_win,0);
					noecho();
					while (!DONE){ // While something to do (No RTS)
						CURRENT_INST=memory[CPU.PC++];
						if( ((*instfunc[CURRENT_INST])(CURRENT_INST))!=0) DONE=1;
						if (BP_FLAG&&(B_POINT==CPU.PC)) DONE=3; // Break point?
						PRINT_REG(); // refresh registers
						if (!DONE) { // If program Break, don't run keyboard or screen handlers
							DONE=KEYBOARD_HANDLER(); // Do something with Keyboard input
						} // End if check
						SCREEN_HANDLER();  // Do something with screen
					}
					echo();  // Echo back on, key press displayed to terminal
					keypad(lcd_win,FALSE);
					switch (DONE){ // What kind of termination happened
						case 1: wprintw(main_win,"Program break\n"); break;
						case 2: wprintw(main_win,"User break\n"); break;
						case 3: wprintw(main_win,"Break Point Reached\n"); break;
						default: break;
					} // End Switch
					wrefresh(main_win);
					break;
		case 's':	// Step a single instruction
					CURRENT_INST=memory[CPU.PC++];
					result=(*instfunc[CURRENT_INST])(CURRENT_INST);
					SCREEN_HANDLER();  // Do something with screen
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
	unsigned char *b = (unsigned char*) &CPU.SR;
	unsigned char byte;
	int i, j;
	mvwprintw(reg_win, 5, 1,"%2x  %2x  %2x  %2x  %4x\t",CPU.A,CPU.X,CPU.Y,CPU.SP,CPU.PC);

	for (j=7;j>=0;j--)
	{
		byte = ((*b) >> j) & 1;
		wprintw(reg_win,"%u ", byte);
	}
	wrefresh(reg_win);
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
		wprintw(main_win,"\n\n Add\tHEX\tDEC\n0x%x:\t%x\t%d\n\n",result1,memory[result1],memory[result1]);		
	}else {
		if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with 0x then base is 16 else base 10
		result1=(int)strtol(tokens[1],NULL,base);  // convert string to integer
		result2=(int)strtol(tokens[2],NULL,base);  // convert string to integer
		k=result1;
		while(k<=result2){		
			wprintw(main_win,"$%x:\t",k); // Print base address
			line=0; i=k; // set up line counter and temp memory counter
			while((line<8)&&(k<=result2)){ // print 8 rows
				wprintw(main_win,"%02x  ",memory[k++]); line++;
			}
			while (i<k) wprintw(main_win,"%c",(memory[i++]<0x20)?('.'):(memory[i-1])); // print memory as characters
			wprintw(main_win,"\n"); // new line at end of row
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
		default:	wprintw(main_win,"\nUnknown Flag in Status Register\n\n");
					break;
	
	}
} // End Mon_SetFlags


static int disk(char *option,char **tokens,int j){

	char flag,*filename,state=0;
	int startadd,endadd,base,k;
	FILE *fp;
	if(strcmp(option,"Load")==0) flag=0;
	if(strcmp(option,"Save")==0) flag=1;

	// set parameters depending on flag
	switch (flag){
	
			case 0:	// Load
					if (j==2) // filename only, use address in file
					{
						filename=tokens[1]; // point filename at first parameter state=0;
					} // End if J==2
					else if (j==3) // address and filename
					{
						filename=tokens[2]; // point filename at second parameter
						if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with # then base is 10 else base 16
						startadd=(int)strtol(tokens[1],NULL,base);  // convert string to integer
						endadd=65535; // set endadd to check condition in loop
						state=1; // Start address given
					} // End if j==3
					else {wprintw(main_win,"Incorrect number of parameters for Load.\nUsage: , {address} <filename>"); return 1;}
					break;
					
			case 1: // Save
					if (j==2) // filename only, save all memory
					{
						filename=tokens[1]; // point filename at first parameter
						startadd=0;
						endadd=65535;
						
					} // end if j==2
					else if (j==3) // start address to end of memory and filename
					{
						filename=tokens[2]; // point filename at second parameter
						if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with # then base is 10 else base 16
						startadd=(int)strtol(tokens[1],NULL,base);  // convert string to integer
						endadd=65535; // set endadd to check condition in loop
						
					} // End if j==3
					else if (j==4) // start address, end address and filename
					{
						filename=tokens[3]; // point filename at second parameter
						if ((tokens)[1][0]=='#') base =10; else base=16; // if parameter starts with # then base is 10 else base 16
						startadd=(int)strtol(tokens[1],NULL,base);  // convert string to integer
						endadd=(int)strtol(tokens[2],NULL,base);  // convert string to integer // set endadd to check condition in loop
					} // End if j==4
					else {wprintw(main_win,"Incorrect number of parameters for Save.\nUsage: , {start address} {end address} <filename>"); return 1;}
					break;
			default: 	wprintw(main_win,"incorrect number of parameters\nUse Help for usage\n");
						return 1;
						break;
	}


	// Open file depending on flag
	if (!(fp=fopen(filename,(flag==0)?"r":"w"))){
		wprintw(main_win,(flag==0)?"Can't Load file\n":"Can't Save file\n");	
	}

	switch (flag){
		case 0:	// Load
				if (state==0) { // Then load start and end address from file
					startadd=getw(fp);
					endadd=getw(fp);
				}
				else { // disgard start and end address
					endadd=getw(fp); // disgard startadd in file, startadd comes from input
					endadd=getw(fp); // get and add from file
				} // end if
				k=startadd;  // use a temporary variable, could use startadd, but...
				if (65536>endadd+k){ // we have enough room from our start memory location to the end memory location
					while(!feof(fp)&&k<65536){  // End of file or k is too big, as eof if a character
						memory[k++]=fgetc(fp); // load memory from the file
					} // End while
				} else { // Not enough room for file contents
					wprintw(main_win,"\n\n ***** File not Loadad, contents too big to fit *****\n");
				} // End if
				break;
		case 1: // Save
				putw(startadd,fp); // save out start address
				putw(endadd,fp); // save out end address
				k=startadd; // temporary variable to walk the array
				if (startadd<endadd && endadd<65536){ // is the user input any good?
					while(k<endadd+1){
						fputc(memory[k++],fp); // save memory to our file
					} // End while
				} // end if 
				break;
	} // End Switch (flag)

	// close file
	if (fp!=NULL) fclose(fp); // if the file was opened, then close it.
	return 0;

}  // End disk




void help(){

	wprintw(main_win,"6510 cli help\n*************\nletters in () are shortcuts\n\n");
	wprintw(main_win,"\nAddresses and values default is hex or if decimal required prefix with #.\nIf the first value is hex, the second is presumed to be hex\n\n");
	wprintw(main_win,", -\t\tLoad memory from file. , {start end address} <filename>\n");
	wprintw(main_win,". -\t\tSave memory to a file. . {start end address} <filename>\n");
	wprintw(main_win,"(a)ssemble -\tAssemble one line at a time: a <start address>\n");
	wprintw(main_win,"(b)reakpoint -\tcreate breakpoint <address>\n\t\t(b) with no arguements will clear breakpoint\n");
	wprintw(main_win,"(d)isssembler -\tDisassemble memory displaying mnemonic: d <start address> [end address]\n");
	wprintw(main_win,"(f)ill -\tFill Memory: f <start address> <end address> <contents>\n");
	wprintw(main_win,"(g)o -\t\tRun code: g on its own will start the code from current PC\n\t\tg <address> runs from a specific address\n");
	wprintw(main_win,"(h)elp -\tthis help screen.  To get help on the command, type the command and <enter>\n");
	wprintw(main_win,"(i)nsert-\tInsert into Memory, i <address> <value>\n");
	wprintw(main_win,"(m)emory -\tMemory dump, can be run as either: m <address>\n\t\tor: m <start address> <end address>\n");
	wprintw(main_win,"(s)tep -\tSingle Step instruction\n");
	wprintw(main_win,"(t)oggle -\tToggle a SR flag: t <flag letter>\n");	
	wprintw(main_win,"(q)uit -\texits program back to shell\n");
	wprintw(main_win,"(x) -\t\te(x)terminate - System Reset\n");
	wrefresh(main_win);
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
            wprintw(main_win,"%u ", byte);
        }
    }
    puts("");
}  // End PRINT_BITS




