/*

Author: Federico Pfaffendorf
EMail: yo@federicopfaffendorf.com.ar
Git: https://github.com/fpfaffendorf/radiomind

C64 Keyboard Pinout
https://www.waitingforfriday.com/wp-content/uploads/2017/01/C64_Keyboard_Schematics_PNG.png

ATmega328 Pinout vs Arduino Pins
https://www.electrosoftcloud.com/wp-content/uploads/2020/04/ATmega328-Pinout-1.png

------------------------------
------------------------------
(I) D08 | White ------------ D      - Col
--
(8) A03 | Black ------------ R      - Row
--
(7) A02 | Grey ------------- R     \
(6) A01 | Purple ----------- R      |
(5) A00 | Blue ------------- R      |
(4) D13 | Green ------------ R      | Rows
(3) D12 | Yellow ----------- R      |
(2) D11 | Orange ----------- R      |
(1) D10 | Red -------------- R      |
(0) D09 | Brown ------------ R     /
(H) D07 | Grey/White ------- D     \
(G) D06 | Purple/White ----- D      |
(F) D05 | Blue/White                |
(E) D04 | Green/White               | Cols
(D) D03 | Yellow/White              |
(C) D02 | Orange/White              |
(B) D01 | Red/White                 |
(A) D00 | Brown/White              /
------------------------------
------------------------------

*/

#define COLS_INIT   0
#define COLS_END    8
#define ROWS_INIT   9
#define ROWS_END    17

#include <NeoSWSerial.h>

NeoSWSerial neoSWSerial(18, 19);
byte keyboardStatus[9][9];

void setup() {

  /* Setup the rows as output high */
  for(byte pin = ROWS_INIT; pin <= ROWS_END; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }

  /* Setup the cols as output high */
  for(byte pin = COLS_INIT; pin <= COLS_END; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }

  /* Begin serial communication @9600bauds */
  neoSWSerial.begin(9600);

  /* Initialize keyboard matrix, all keys up */
  initKeyboardMatrix();

}

void loop() {

  /* Iterate cols */
  for(byte pinOut = COLS_INIT; pinOut <= COLS_END; pinOut++) {

    /* Set LOW the current output line */
    digitalWrite(pinOut > COLS_INIT ? pinOut - 1 : COLS_END, HIGH);
    digitalWrite(pinOut, LOW);

    /* Iterate rows */
    for(byte pinIn = ROWS_INIT; pinIn <= ROWS_END; pinIn++) {

      /* Output serial on key down */
      byte col = pinOut - COLS_INIT;
      byte row = pinIn - ROWS_INIT;

      /* Configure current pin as input pull up */
      byte lastPinIn = pinIn > ROWS_INIT ? pinIn - 1 : ROWS_END;
      pinMode(lastPinIn, OUTPUT);
      digitalWrite(lastPinIn, HIGH);
      pinMode(pinIn, INPUT_PULLUP);      
      
      /* Key pressed */     
      if (!digitalRead(pinIn)) 
      {
        /* Key wasn't pushed before, it is a change */
        if(!keyboardStatus[col][row]) {
          /* Output key down event */
          outputSerialStatus(col, row, true);
          /* Set key status as down */
          keyboardStatus[col][row] = true;
        }
      }
      /* Output serial on key up */
      else
      {
        if(keyboardStatus[col][row])
        {
          /* Output key down event */
          outputSerialStatus(col, row, false);
          /* Set key status as up */
          keyboardStatus[col][row] = false;
        }
      }
      
    }
    
  }

  /* Process incomming messages if any */
  while(neoSWSerial.available()) 
  {
    /* Execute command */
    switch(neoSWSerial.read()) {
      case 0x01: {
        initKeyboardMatrix();
      }
    }
  }

}

void initKeyboardMatrix()
{
  /* Initialize keyboard status as all keys up */
  for(byte col = 0; col <= 9; col++)
    for(byte row = 0; row <= 9; row++) {
      keyboardStatus[col][row] = false; 
    } 
}

void outputSerialStatus(byte col, byte row, bool status)
{
  /* Col ASCII Serial output */
  neoSWSerial.write(col + 65);
  /* Row ASCII Serial output */
  neoSWSerial.write(row + 48);
  /* Status Down */
  neoSWSerial.write(status ? 'D' : 'U');
}

