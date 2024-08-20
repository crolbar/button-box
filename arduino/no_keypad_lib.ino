#include <HID-Settings.h>
#include <HID-Project.h>


#define buttons_num 18
#define coll_num 7
#define rows_num 3

const int encA1 = 2;
const int encB1 = 3;

const int encA2 = 1;
const int encB2 = 0;

volatile int encoderPos1 = 0;
volatile int lastEncoded1 = 0;
volatile long lastEncoderValue1 = 0;

volatile int encoderPos2 = 0;
volatile int lastEncoded2 = 0;
volatile long lastEncoderValue2 = 0;
 
bool encoder1 = false;
bool encoder2= false;
bool black1 = false;
bool black2 = false;
bool red1 = false;
bool red2 = false;
bool silver1 = false;
bool silver2 = false;
bool silver3 = false;
bool silver4 = false;
bool silver5 = false;
bool toggle1 = false;
bool toggle2 = false;
bool momen1up = false;
bool momen1d = false;
bool momen2up = false;
bool momen2d = false;
bool ledb = false;
bool no = false;


int rows[rows_num] = {16, 14, 15};
int coll[coll_num] = {4, 5, 6, 7, 8, 9, 10};



bool* pressed_buttons[rows_num][coll_num] = {
    {&toggle1, &toggle2, &momen1up, &momen2up, &ledb, &momen1d, &momen2d},
    {&silver1, &silver2, &silver3, &silver4, &silver5, &no, &no},
    {&encoder2, &black1, &black2, &red1, &red2, &encoder1, &no},
};
bool debouncer[rows_num][coll_num] = {
    {false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false},
};    



void setup() {
    setup_pins();
}

void loop() {
    checkenc(); 
    checkbutton();
}

void checkbutton(){
  update_buttons();

    if (encoder1) {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_F7);
        Keyboard.releaseAll();
    }
    if (encoder2) {
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_F7);
        Keyboard.releaseAll();
    }
    if (black1) {
        Consumer.write(MEDIA_PREVIOUS);
    }
    if (black2) {
         Consumer.write(MEDIA_NEXT);
    }
    if (red1) {
        Consumer.write(MEDIA_PLAY_PAUSE);
    }
    if (red2) {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('d');
        Keyboard.releaseAll();
    }
    if (silver1) {
        Keyboard.println("silver1");
    }
    if (silver2) {
        Keyboard.println("silver2");
    }
    if (silver3) {
        Keyboard.println("silver3");
    }
    if (silver4) {
        Keyboard.println("silver4");
    }
    if (silver5) {
        Keyboard.println("silver5");
    }
    if (toggle1) {
      Keyboard.press('6');
      Keyboard.press('5');
      Keyboard.press('3');
      Keyboard.press('0');
      Keyboard.press('4');
      
      Keyboard.releaseAll();

    }
    if (toggle2) {
        Keyboard.println("toggle2");
    }
    if (momen1up) {
        Keyboard.println("momen1up");
    }
    if (momen1d) {
        Keyboard.println("momen1d");
    }
    if (momen2up) {
        Keyboard.println("momen2up");
    }
    if (momen2d) {
        Keyboard.println("momen2d");
    }
    if (ledb) {
        Keyboard.println("ledb");
    }
    
}
void checkenc(){
  if (lastEncoderValue1 != encoderPos1) {
        if (encoderPos1 > lastEncoderValue1) {
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.press(KEY_F3);
            Keyboard.releaseAll();
        } else {
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.press(KEY_F5);
            Keyboard.releaseAll();
        }
        lastEncoderValue1 = encoderPos1;
    }


    if (lastEncoderValue2 != encoderPos2) {
        if (encoderPos2 > lastEncoderValue2) {
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.press(KEY_F1);
            Keyboard.releaseAll();
        } else {
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.press(KEY_F2);
            Keyboard.releaseAll();
        }

        lastEncoderValue2 = encoderPos2;
    }
}

void updateEncoder1() {
    int MSB = digitalRead(encA1);
    int LSB = digitalRead(encB1);

    int encoded = (MSB << 1) | LSB;
    int sum = (lastEncoded1 << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        encoderPos1++;
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        encoderPos1--;
    }

    lastEncoded1 = encoded;
}

void updateEncoder2() {
    int MSB = digitalRead(encA2);
    int LSB = digitalRead(encB2);

    int encoded = (MSB << 1) | LSB;
    int sum = (lastEncoded2 << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        encoderPos2++;
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        encoderPos2--;
    }

    lastEncoded2 = encoded;
}

void update_buttons() {
    for (int ri = 0; ri < rows_num; ri++) {
        digitalWrite(rows[ri], LOW);
        delay(10);
        for (int ci = 0; ci < coll_num; ci++) {
            *pressed_buttons[ri][ci] = digitalRead(coll[ci]) == LOW && !debouncer[ri][ci];

            debouncer[ri][ci] = 
                (digitalRead(coll[ci]) == LOW && !debouncer[ri][ci]) ? true :
                (digitalRead(coll[ci]) == HIGH && debouncer[ri][ci]) ? false :
                debouncer[ri][ci];
        };
        digitalWrite(rows[ri], HIGH);
    }
}

void setup_pins() {
    pinMode(encB1, INPUT_PULLUP);
    pinMode(encA1, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(encA1), updateEncoder1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encB1), updateEncoder1, CHANGE);

    pinMode(encB2, INPUT_PULLUP);
    pinMode(encA2, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(encA2), updateEncoder2, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encB2), updateEncoder2, CHANGE);

    for (int i = 0; i < rows_num; i++) {
        pinMode(rows[i], OUTPUT);
    }
    for (int i = 0; i < coll_num; i++) {
        pinMode(coll[i], INPUT_PULLUP);
    }
}
