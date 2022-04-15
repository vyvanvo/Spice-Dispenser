//4x3 keypad (no letters)

//keypad library
#include <Keypad.h>
//lcd library
#include <LiquidCrystal.h>

//initialize keypad
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

//initialize lcd 
const byte rs = 53, en = 51, d4 = 49, d5 = 47, d6 = 45, d7 = 43;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // put your setup code here, to run once:
  
  //initialize serial monitor
  Serial.begin(9600);

  //initialize lcd
  lcd.begin(16, 2); //set up lcd's number of columns and rows
  lcd.print("key pressed:");
  lcd.setCursor(0,1);
}

void loop() {
  // put your main code here, to run repeatedly:

  //set cursor to column 0, row 1
  //lcd.setCursor(0,1);

  //get the character of the button pressed
  char key = keypad.getKey();

  //key = 0 if no buttons are pressed
  if(key) {
    lcd.print(key);
    Serial.println(key);
  }
}
