#include <stdio.h>
#include <stdarg.h>

#define RESET 0
#define BRIGHT 1
#define DIM 2
#define CURSIVE 3
#define UNDERLINE 4
#define BLINK 5
#define REVERSE	7
#define HIDDEN 8

#define BLACK 0
#define RED	1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define	WHITE 7

void textcolor(int attr, int fg, int bg);
void resetcolor();


int infof(const char *format, ...){
	textcolor(BRIGHT, WHITE, BLUE);
	va_list arguments;
	va_start(arguments, format);
	vprintf(format, arguments);
	va_end(arguments);
	resetcolor();

}

int warnf(const char *format, ...){
	textcolor(CURSIVE, YELLOW, BLACK);
	va_list arguments;
	va_start(arguments, format);
	vprintf(format, arguments);
	va_end(arguments);
	resetcolor();
}

int errorf(const char *format, ...){
	textcolor(UNDERLINE, RED, BLACK);
	va_list arguments;
	va_start(arguments, format);
	vprintf(format, arguments);
	va_end(arguments);
	resetcolor();
}

int panicf(const char *format, ...){
	textcolor(BLINK, CYAN, BLACK);
	va_list arguments;
	va_start(arguments, format);
	vprintf(format, arguments);
	va_end(arguments);
	resetcolor();
}

void textcolor(int attr, int fg, int bg){
	printf("%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
}

void resetcolor(){
	textcolor(RESET, WHITE, BLACK);
	fflush(stdout);
}