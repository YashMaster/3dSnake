# MPLAB IDE generated this makefile for use with GNU make.
# Project: snake.mcp
# Date: Thu Sep 23 13:28:55 2010

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

snake.cof : main.o
	$(LD) /p18F4520 /l"C:\MCC18\lib" "main.o" /u_CRUNTIME /z__MPLAB_BUILD=1 /o"snake.cof" /M"snake.map" /W

main.o : main.c C:/MCC18/h/Delays.h gamecube.h C:/MCC18/h/stdlib.h snake.h main.c C:/MCC18/h/p18f4520.h C:/MCC18/h/p18cxxx.h
	$(CC) -p=18F4520 "main.c" -fo="main.o" -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "main.o" "snake.cof" "snake.hex"

