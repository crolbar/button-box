#include <HID-Project.h>
#include <HID-Settings.h>
#include <Keypad.h>
#define NUMBUTTONS 20
#define NUMROWS 3
#define NUMCOLS 8
#define REVERSED false
// buttons
const byte ONETS1_UP = '1';
const byte ONETS2_UP = '2';
const byte TS1_UP = '3';
const byte TS2_UP = '4';
const byte TS3_UP = '5';
const byte TS1_DOWN = '6';
const byte TS2_DOWN = '7';
const byte TS3_DOWN = '8';
const byte blk1 = '9';
const byte blk2 = "10";
const byte blk3 = "11";
const byte blk4 = "12";
const byte blk5 = "13";
const byte s1 = "14";
const byte s2 = "15";
const byte s3 = "16";
const byte red_bt = "17";
const byte red_tp = "18";

// potentiometers
int potenI = 0;
int poten2I = 0;
int prevpotenI = 0;

int potenII = 0;
int poten2II = 0;
int prevpotenII = 0;


// define the buttons in the matrix grid
byte buttons[NUMROWS][NUMCOLS] = {
  { ONETS1_UP,ONETS2_UP,TS1_UP,TS2_UP,TS3_UP,TS1_DOWN,TS2_DOWN,TS3_DOWN },
  { blk1,blk2,blk3,blk4,blk5 }, 
  { s1,s2,s3,red_bt,red_tp }
  
};

// define the collums and rows in the matrix grid
byte rowPins[NUMROWS] = {14,16,10};
byte colPins[NUMCOLS] = {9,8,7,6,5,4,3,2}; 
Keypad buttx = Keypad( makeKeymap(buttons), rowPins, colPins, NUMROWS, NUMCOLS );

// initialize the button-box
void setup(){
  Keyboard.begin();
  delay(500);  
}

//  loop to check if a button is pressed or a potentiometer is turned
void loop(){
  CheckALLButtonG();
  CheckALLPoten();
}

// define what keys to press if a button is pressd
void CheckALLButtonG(void){  //===========================BUTTONS==========================
  byte key = buttx.getKey();
  byte keystate = buttx.getState(); 

    if (key == s1){                  //
    


    }
    if (key == s2){                  //



    }
    if (key == s3){                  //
      


    } 
    if (key == red_bt){                   //



    }
    if (key == red_tp){                   // 



    } 
    if (key == TS3_DOWN){               //



    }
    if (key == ONETS1_UP){              //



    }
    if (key == ONETS2_UP){              //



    }
    if (key == TS1_UP){                 //



    }
    if (key == TS2_UP){                 //



    }
    if (key == TS3_UP){                 //

    
    }
    


    if (key == TS1_DOWN){               //



    }
    if (key == blk1){                   //     



    }
    if (key == blk2){                   //


    }
    if (key == blk3){                   //



    }
    if (key == blk4){                   //



    }
    if (key == blk5){                   //



    }
    if (key == TS2_DOWN){               //


    }
} 

void CheckALLPoten(void) { //============================POTENTIOMETERS==================
  potenI = analogRead(A1);
  potenII = analogRead(A0);
  potenI = map(potenI, 0, 1023, 0, 101);          
  potenII = map(potenII, 0, 1023, 0, 101);    
    if(REVERSED) {
      potenI = 101 - potenI;
    }
    if(REVERSED) {
       potenII = 101 - potenII;
    }

        //potentiometer 1


    if ( abs(potenI - prevpotenI) > 1) {          //check if potentiometer potenIue has changed
        prevpotenI = potenI;
        potenI /= 2;                               //divide it by 2 to get 51 steps
        while(poten2I < potenI) {  //if the poten is turned left do.. //              




          poten2I++;                   
          delay(2);
        }
        while(poten2I > potenI) { //if the poten is turned right do.. // 




          poten2I--;
          delay(2);
          }
        }

          //potentiometer 2


    if ( abs(potenII - prevpotenII) > 1) {         
      prevpotenII = potenII;
      potenII /= 2;                              
      while(poten2II < potenII) {  //if the poten is turned left do.. //

          


          poten2II++;
          delay(2);
      }
      while(poten2II > potenII) { //if the poten is turned right do.. //




          poten2II--;
          delay(2);
        }
      }
}
