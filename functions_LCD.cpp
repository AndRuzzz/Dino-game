#include "functions_LCD.h"

//char* clearDisplay = "00000001";
//char* returnHome = "00000010";
//char* displayon = "00001100";
//char* twoLineDisplay = "00111100";


void wData (bool rs, char * datapin_table){ //do a specific task written in the datasheet 
  char * p = datapin_table;
  if (rs) PORTB |= B010000; else  PORTB &= B101111; //set register select to 0 or 1
        
    for (int i = 3; i<8;i++)  {
    if (*p == '1') PORTD |= (B00000001 << (i)); else PORTD = PORTD & (~(B00000001 << (i))); //put pins D0-D5 to desired state
    p++;    
      
  }
  for (int i = 0; i<3;i++) {
    if (*p == '1') PORTB |= (B000001 << i); else PORTB = PORTB & (~(B000001 << i)); //put pins D5-D7 to desired state
    p++;
    
  }
  PORTB |= B001000;
  PORTB &= B110111;
 
}


void initt (){   //set starting parameters of the LCD 
  DDRD |= B11111000;  //set I/O pins to the desired state
  DDRB |= B011111;
  wData (0,"00111100");  //set LCD to two line mode
  wData (0,"00000001");   //clear LCD
  wData (0,"00000010");     //set Cursor to the start 
  wData (0,"00001100");      //turn the display on
}


void moveCursor (unsigned int x, unsigned int y){ //move cursor till horizontal position equals x and vertical position equals y
  
  wData (0,"00000010");
  if (y == 1) wData (0,"11000000");  //if y equals 1, move the cursor down
  for (int i = 0;i<x;i++){
    wData (0,"00010100");           //move the cursor in the horizontal position till it reaches desired position
  }
  
}



char* charToString (char a){ //change char to string so it can be read by the main function writeData
  char* stringg;
  for (int i = 0;i<8;i++){
    if (((B00000001 <<i) & a) != 0) stringg[7-i] = '1'; else stringg[7-i] = '0';
  }
  return stringg;
}



void writeWord (char* word){ //display sentence/word
  char* p = word;
  while (*p!='\0'){
    wData (1,charToString(*p));
    p++;
  }
  
}


unsigned long reverseNum(unsigned long n) //reverse the number so it can be correctly used in the function printNumber which would output the number in the reverse order
{
  
   unsigned long sum=0;
   while (n!=0)
   {
     sum = sum*10 + n%10;
     n /= 10;
   }
   return sum;
}



void printNumber (unsigned long num){ //display a number
  uint8_t temp;
  unsigned long check_10 = num;
  num = reverseNum(num);
  while (num != 0){
    temp = num % (10);
    switch (temp){
     case 0: writeWord ("0"); break;
     case 1: writeWord ("1");break;
     case 2: writeWord ("2");break;
     case 3: writeWord ("3");break;
     case 4: writeWord ("4");break;
     case 5: writeWord ("5");break;
     case 6: writeWord ("6");break;
     case 7: writeWord ("7");break;
     case 8: writeWord ("8");break;
     case 9: writeWord ("9");break;
   }
    num = num - temp;
    num = num/10;
  }
  if (check_10%10 == 0) writeWord ("0"); //if the number is divadable by 10 then the reverse function would not return the proper number (example: 10 ---> 1). 

}

