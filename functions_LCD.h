#include "Arduino.h"
#ifndef functions_LCD_h
#define functions_LCD_h



void wData (bool rs, char * datapin_table);
void moveCursor (unsigned int x, unsigned int y);
char* charToString (char a);
void writeWord (char* word);
void initt ();
void printNumber (unsigned long num);
unsigned long reverseNum(unsigned long n);


#endif