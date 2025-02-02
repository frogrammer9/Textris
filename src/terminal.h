#ifndef TERMINAL_H
#define TERMINAL_H
#include <stdint.h>
#include <stdio.h>

#define FLUSHOUT fflush(stdout)

#define DEFAULT_B		 "\e[49m" 
#define BLACK_B			 "\e[40m" 
#define DARK_RED_B		 "\e[41m" 
#define DARK_GREEN_B	 "\e[42m" 
#define DARK_YELLOW_B	 "\e[43m" 
#define DARK_BLUE_B		 "\e[44m" 
#define DARK_MAGENTA_B	 "\e[45m" 
#define DARK_CYAN_B		 "\e[46m" 
#define LIGHT_GRAY_B	 "\e[47m" 
#define DARK_GRAY_B		 "\e[100m"	
#define RED_B			 "\e[101m"	
#define GREEN_B			 "\e[101m"	
#define ORANGE_B		 "\e[103m"	
#define BLUE_B			 "\e[104m"	
#define MAGENTA_B		 "\e[105m"	
#define CYAN_B			 "\e[106m"	
#define WHITE_B			 "\e[107m" 
#define DEFAULT_F		 "\e[39m"  
#define BLACK_F			 "\e[30m"  
#define DARK_RED_F		 "\e[31m"  
#define DARK_GREEN_F	 "\e[32m"  
#define DARK_YELLOW_F	 "\e[33m"  
#define DARK_BLUE_F		 "\e[34m"  
#define DARK_MAGENTA_F	 "\e[35m"  
#define DARK_CYAN_F		 "\e[36m"  
#define LIGHT_GRAY_F	 "\e[37m"  
#define DARK_GRAY_F		 "\e[90m" 
#define RED_F			 "\e[91m" 
#define GREEN_F			 "\e[92m" 
#define ORANGE_F		 "\e[93m" 
#define BLUE_F			 "\e[94m" 
#define MAGENTA_F		 "\e[95m" 
#define CYAN_F			 "\e[96m" 
#define WHITE_F			 "\e[97m" 

int terminal_setup(uint8_t* charC_out, uint8_t* lineC_out, uint8_t* draw_frame_flag);

void setchar(uint8_t x, uint8_t y, const char* fore, const char* back, char c);
void setstr(uint8_t x, uint8_t y, const char* fore, const char* back, const char* s);

#endif
