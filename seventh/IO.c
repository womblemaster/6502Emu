#include "fsm.h"

/*
 * Because we have used NCurses cbreak() function configured in fsm.h SYS_RESET()
 * we should be able to take keyboard input during program execution, interpret it
 * and action the result
 */
int KEYBOARD_HANDLER(){
	

	char ch=wgetch(lcd_win); // Character to get

	if (ch == KEY_F(1)) {return 2;}
	if (ch == '`') {return 2;} // User break requested

	return 0;
}


/*
 * The screen is a pretend LCD screen.  It has two registers mapped to memory.
 * I have mapped them as follows:
 * 
 * $1000: Character to display
 * $1001: 8 bit instruction
 * 
 * The Character to display is just a 1 byte character
 * The 8 bit instruction breaks down as follows
 * 		10000000	128		Something to do
 * 		01000000	 64		Clear Screen
 * 		00100000	 32		Print Character
 * 		00010000	 16		Invert Character at cursor
 * 		00001000	  8		Move Cursor: 8 + 0 Move cursor Forward one
 * 		00000100	  4		8+4 Move Cursor back one
 * 		00000010	  2		8+2 Move Cursor Up one line (can be ignored if y=0)
 * 		00000001	  1		8+1 Move Cursor Down one line (will scroll screen if at bottom)
 * 
 * So if memory[0x1001]&128 there is something to do
 * Check all other bits and action accordingly
 * Clear memory[0x1001];
 * 
 * This routine has a static internal structure to maintain the LCD Screen
 * All maths are done based on the #define in fsm.h
 * 
 */
void SCREEN_HANDLER(){	
	uchar scr_reg=memory[E_LCD_REG],mask=scr_reg&7;
	uchar scr_char=memory[E_LCD_DATA];
	int j;

	if((scr_reg>>7)&1) // there is something to do
	{	
		memory[E_LCD_REG]=0; // Clear the register location
		if((scr_reg>>6)&1){	// Clear Screen
			wclear(lcd_win); // Clear window
		} // End if &64
		if((scr_reg>>5)&1){	// Print Character
			wprintw(lcd_win,"%c",scr_char);
		} // End if &32
		
		if(scr_reg&16){	// Invert Character
		} // End if &16
		
		if(scr_reg&8){	// Move Cursor
			switch(mask){ // Move Where
				case 12: break; // Back one
				case 10: break; // Up one
				case 9:  break; // Down one
				case 8:  break; // Forward one
				default: break; // do nothing, bad mask
			}
		} // End if &8
	} // end if
	wrefresh(lcd_win);
	wrefresh(main_win);
}
