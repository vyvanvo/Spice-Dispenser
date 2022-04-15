//4x3 keypad (no letters)

//keypad library
#include <Keypad.h>

//initializing keypad
const byte ROWS = 4; //4 rows
const byte COLS = 4; //4 columns

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte row_pins[ROWS] = {37, 35, 33, 31}; //connect to the row pinouts of the keypad
byte col_pins[COLS] = {29, 27, 25, 23}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), row_pins, col_pins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //get the character of the button pressed
  char key = keypad.getKey();

  //key = 0 if no buttons are pressed
  if(key) {
    Serial.println(key);
  }
}
