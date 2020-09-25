#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include "logger.h"

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
#define DEFAULT_BACK 8

void textcolor(int attr, int fg, int bg);
void resetcolor();
void printmsg(const char *format, va_list args);

int isSyslog = 0;
int priority = 0;

int initLogger(char *logType) {
	if(strcmp(logType, "syslog") == 0){
		isSyslog = 1;
	}
    printf("Initializing Logger on: %s\n", logType);
    return 0;
}

int infof(const char *format, ...) {
	textcolor(BRIGHT, WHITE, BLUE);
	priority = LOG_INFO;
	va_list arguments;
	va_start(arguments, format);
	printmsg(format, arguments);
	va_end(arguments);
	resetcolor();
    return 0;
}

int warnf(const char *format, ...) {
	textcolor(CURSIVE, YELLOW, BLACK);
	priority = LOG_WARNING;
	va_list arguments;
	va_start(arguments, format);
	printmsg(format, arguments);
	va_end(arguments);
	resetcolor();
    return 0;
}

int errorf(const char *format, ...) {
	textcolor(UNDERLINE, RED, BLACK);
	priority = LOG_ERR;
	va_list arguments;
	va_start(arguments, format);
	printmsg(format, arguments);
	va_end(arguments);
	resetcolor();
    return 0;
}
int panicf(const char *format, ...) {
	textcolor(BLINK, CYAN, BLACK);
	priority = LOG_CRIT;
	va_list arguments;
	va_start(arguments, format);
	printmsg(format, arguments);
	va_end(arguments);
    return 0;
}

void printmsg(const char *format, va_list args){
	if(isSyslog){
		openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
		vsyslog(priority, format, args);
		closelog();
	} else {
		vprintf(format, args);
	}
}

void textcolor(int attr, int fg, int bg){
	printf("%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
}

void resetcolor(){
	textcolor(RESET, WHITE, DEFAULT_BACK);
	fflush(stdout);
}