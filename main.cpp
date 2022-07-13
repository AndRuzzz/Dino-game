#include <EEPROM.h>
#include <time.h>
#include <math.h>

//pin numbers of buttons and their functions
#define CONFIRM 1        //confirm selection  
#define SWITCH_ARROW 13  //move cursor vertically 
#define SWITCH_CHAR 13   //change characters when picking a name
#define JUMP 2           //jump when button is pressed


//GAME MODELS
char* characterModel = "10010101";    
char* cactus = "11110011";

//global variables
static uint8_t  dino_pos_x = 3;  //coordinates of dinosaur position
static uint8_t  dino_pos_y = 1;  
static uint8_t  cactus_pos = 16;  
static uint8_t cactus2_pos = 16+7;
unsigned int score1;            
unsigned int score2;
char letter_library[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','r','s','t','u','v','z','0'};


//function library
void doJump (void);
char set_score (unsigned int score);
unsigned int eepromToUint (unsigned int prvi, unsigned int drugi);
void pause (unsigned long time);


void setup() {
 initt();   //initialise the LCD
 DDRD = DDRD & (~B00000110); //SET OUTPUT AND 
 DDRB = DDRB & (~B100000);   //INPUT PINS
 Serial.begin (9600);
 attachInterrupt(digitalPinToInterrupt(JUMP),doJump ,FALLING);
 randomSeed(analogRead(0));
}



void loop() {
  //variable declaration
  char i,m,e;
  unsigned int displayit, displayit2;
  unsigned int end_score;
  static bool starting_menu = 1;
  static char ime_igralca[3];
  unsigned int best_score;
  bool pick = 0;
  char end_game;
  unsigned int score = 0;
  static int count_jump = 0;
  
  //reset parameters
  writeData (0,clearDisplay);
  dino_pos_x = 3;
  dino_pos_y = 1;
  cactus_pos = 16;
  cactus2_pos = 16+7;
  
  //print starting menu
  moveCursor (0,0);
  writeWord (">");
  moveCursor(1,0);
  writeWord ("Start");
  moveCursor (1,1);
  writeWord ("Score");
  
  //change arrow position to pick desired option (start game/score) and confirm your pick
  while ((PIND & B00000010) != 0) {
   if ((PINB & B100000) == 0){
    moveCursor (0,pick);
    writeWord (">");
    moveCursor (0,!pick);
    
    pick = !pick;
    Serial.println(pick);
    writeWord (" ");
    pause (100);
    }
  }
  
 
  while (1){
    
  //set player name 
  if (pick == 1){
    
    char *p = letter_library;
    
    writeData (0,clearDisplay);
    pause (200);
    moveCursor (0,0);
    writeWord ("Name: ");

   for (int j = 0;j<3;j++){ //loop while 3 characters have not been picked
     
     p = letter_library;
     while ((PIND & B00000010)!=0){ 
       moveCursor (j+6,0);
       writeData (1,charToString(*p));
       moveCursor (j+6,0);
      if ((PINB & B100000) == 0){
         p++;
        if (*p == '0') p = letter_library;
        writeData (1,charToString(*p));
        
      } 
        pause (200);  
     }
      pause (200);
    }
    
  
  writeData (0, clearDisplay);
 
 
  //start the game
  while (1){
    end_game = set_score (score);
    moveCursor (dino_pos_x,dino_pos_y);
    writeData (1,characterModel);
    
    if (dino_pos_y == 0){ 
    
      moveCursor (dino_pos_x,1);
      writeWord (" ");
      if (count_jump == 3){   //if dino has been in the air for 3 steps move him down
        moveCursor (dino_pos_x, dino_pos_y);
        writeWord (" ");
        dino_pos_y = 1;
        moveCursor (dino_pos_x, dino_pos_y);
        writeData (1,characterModel);
        count_jump = 0;
      }
      else count_jump++;
    }
    if (((dino_pos_x == cactus_pos) && (dino_pos_y == 1))|| ((dino_pos_x==cactus2_pos) && (dino_pos_y == 1))) end_game = 'e'; 
    moveCursor (cactus_pos+1,1);
    writeWord (" ");
    if (cactus2_pos >15){
      
    }
    else{
      
     moveCursor (cactus2_pos+1,1);
     writeWord (" ");
     }
    moveCursor (cactus_pos,1);
    writeData (1,cactus);
    if (cactus2_pos > 15){
    }
    
    else{
      moveCursor (cactus2_pos,1);
      writeData (1,cactus);
    }
  
    if (end_game == 'e'){
      writeData (0,clearDisplay);
      moveCursor (0,0);
      writeWord ("GAME OVER");
      moveCursor (0,1);
      writeWord ("SCORE: ");
      printNumber (score);
      pause (10000);
      end_score = score;
      break;
    }
    if ((dino_pos_x == cactus_pos)|| (dino_pos_x == cactus2_pos)) score++;

    //set difficulty based on the score
    unsigned long difficulty;
    
    if (score<=10) difficulty = 500;
    if ((score>10)&&(score<=50)) difficulty = 300;
    if ((score>50)&&(score<=100))difficulty = 200;
    if ((score>100)) difficulty = 100;
    pause (difficulty);
     if (cactus_pos==0){
      
      moveCursor (0,1);
      writeWord (" ");
      cactus_pos = 16;
    }
    if (cactus2_pos == 0){
      moveCursor (0,1);
      writeWord (" ");
      cactus2_pos = 16+(random(0,5));
    }
   

    cactus_pos--;
    cactus2_pos--;
    
  }
 end_score = score;
    score1 = (end_score & 0x00FF); //divide score into two seperate scores so it can be saved in the EEPROM
    score2 = (end_score & 0xFF00);
    score2 = score2>>8;
    
    
    if (end_score > eepromToUint(EEPROM.read(4),EEPROM.read(3))){
    //save name and score into EEPROM
    EEPROM.write (0,ime_igralca[0]);
    EEPROM.write (1,ime_igralca[1]);
    EEPROM.write (2,ime_igralca[2]);
    EEPROM.write (3,score2);
    EEPROM.write (4,score1);
    }
   break;
  }
    
  else if (pick == 0){ 
    displayit = EEPROM.read(3);
    displayit2 = EEPROM.read(4);
    best_score = eepromToUint(displayit2,displayit);
    writeData (0,clearDisplay);
    moveCursor (0,0);
    writeWord ("NAME: ");
    i = EEPROM.read (0);
    printNumber (i);
    m = EEPROM.read (1);
    printNumber (m);
    e = EEPROM.read (2);
    printNumber (e);
    moveCursor (0,1);
    writeWord ("SCORE: ");
    printNumber(best_score);
    pause (5000);
    
    break;
    
  }
 }
}

void doJump (void){
 dino_pos_y = 0;
}

  
  
unsigned int eepromToUint (unsigned int x, unsigned int y){
  //extract scores from EEPROM and put them together so it can be displayed
  unsigned int power = 0;
  unsigned int sum = 0;
  unsigned int shift = 0;

  for (power; power<8;power++){
    if (x & (1<<shift)){
      sum = sum + (unsigned int) (pow (2,power)+0.5);
    }
    shift++;
    
  }
  shift = 0;
  for (power;power<16;power++){
    if (y & (1<<shift)){
      sum = sum + (unsigned int)  (pow (2,power)+0.5);
    }
    shift++;
  }
  return sum;
}


char set_score (unsigned int score){
  if (score >= 60000) return 'e'; //end the game
  if (score / 10 == 0){
    moveCursor (15,0);
    printNumber (score);
   
  }
  else if (((score/10)>0) && ((score/10)<10)){
    moveCursor (14,0);
    printNumber (score);
  }
  else if (((score/10)>9) && ((score/10)<100)){
    moveCursor (13,0);
    printNumber (score);
  }
  else if (((score/10)>99) && ((score/10)<1000)){
    moveCursor (12,0);
    printNumber (score);
  }
  else if (((score /10)>999) && ((score/10)<60000)){
    moveCursor (11,0);
    printNumber (score);
  }
 return 'p';
}

void pause (unsigned long time){
  unsigned long current = millis();
  while ((millis()-current)<= time){};
}


