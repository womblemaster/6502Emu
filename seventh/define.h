// program defines
#define RAM 65536 // Amount of RAM addressable with 16 bits of program counter
#define LCD_HEIGHT 8 // LCD Screen height, border adds 2
#define LCD_WIDTH 20 // LCD Screen width, border adds 2
#define LCD_BORDER_W LCD_WIDTH+2
#define LCD_BORDER_H LCD_HEIGHT+2
#define SCREEN_MEM LCD_HEIGHT*LCD_WIDTH // screen memory size
#define STD_BORDER 0 // normal border type



// Emulator environemnt defines
#define E_LCD_DATA	0x1000
#define E_LCD_REG	0x1001
