//Measurement Input SM

//4x3 keypad (no letters)

//keypad library
#include <Keypad.h>
//lcd library
#include <LiquidCrystal.h>

//initialize keypad
const byte ROWS = 4; //4 rows
const byte COLS = 4; //4 columns

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte row_pins[ROWS] = {37, 35, 33, 31}; //connect to the row pinouts of the keypad
byte col_pins[COLS] = {29, 27, 25}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), row_pins, col_pins, ROWS, COLS);

//initialize lcd 
const byte rs = 53, en = 51, d4 = 49, d5 = 47, d6 = 45, d7 = 43;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Measurement Input SM

unsigned char dispense = 0; //global variable
unsigned long measurement = 0;

enum MeasurementInputSM_states {MI_Start, MI_Wait, MI_PressKey, MI_EndofPress, MI_Backspace, MI_Enter} MI_state;

void MeasurementInputSM_Tick() {

  //get the character of the button pressed
  char key = keypad.getKey();
  // key = 0 --> no buttons pressed

  static int keypad_inputs[3] = {0, 0, 0};
  static unsigned char k = 0; //iterator for keypad input array ({H, T, O})
  static unsigned char kp_input_places = 0;
  static unsigned char col_cursor = 7;

  //transition
  switch(MI_state) {
    case MI_Start:
      MI_state = MI_Wait;
      break;

    case MI_Wait:
      if (key == 0) {
        MI_state = MI_Wait;
      }
      else if (key == '*') {
        MI_state = MI_Backspace;

        if (col_cursor > 7) {
          col_cursor--;
          lcd.setCursor(col_cursor, 0);
          lcd.print(' ');
          
          keypad_inputs[k] = 0;

          if (k > 0) {
            k--;
          }

          if (kp_input_places > 0) {
            kp_input_places--;
          }
          //Serial.print(kp_input_places, HEX);
        }
        
      }
      else if (key == '#') {
        MI_state = MI_Enter;
        dispense = 1;

        if (kp_input_places == 1) {
          measurement = keypad_inputs[0];
          Serial.print(measurement);
        }
        else if(kp_input_places == 2) {
          measurement = (keypad_inputs[0] * 10) + keypad_inputs[1];
          Serial.print(measurement);
        }
        else if (kp_input_places == 3) {
          measurement = (keypad_inputs[0] * 100) + (keypad_inputs[1] * 10) + (keypad_inputs[2]);
          Serial.print(measurement);
        }
        else {
          measurement = 0;
          Serial.print(measurement);
        }
        
      }
      else { //number key pressed
        if (col_cursor < 10) {
          MI_state = MI_PressKey;

          lcd.setCursor(col_cursor, 0);
          lcd.print(key);
          col_cursor++;

          keypad_inputs[k] = key - 48;
          //Serial.println(keypad_inputs[k], DEC);
          if (k < 2) {
            k++;
          }

          if (kp_input_places < 3) {
            kp_input_places++;
          }
          //Serial.print(kp_input_places, HEX);

        }
        else {
          MI_state = MI_EndofPress;
        }
      }

      break;

    case MI_PressKey:
      if (key == 0) {
        MI_state = MI_Wait;
      }
      else {
        MI_state = MI_PressKey;
      }
      break;

    case MI_EndofPress:
      if (key == 0) {
        MI_state = MI_Wait;
      }
      else {
        MI_state = MI_EndofPress;
      }
      break;

    case MI_Backspace:
      if (key == '*') {
        MI_state = MI_Backspace;
      }
      else {
        MI_state = MI_Wait;
      }
      break;

    case MI_Enter:
      if (key == '#') {
        MI_state = MI_Enter;
      }
      else {
        MI_state = MI_Wait;
        dispense = 0;
      }
      break;

    default:
      break;

  }

  //action
  switch(MI_state) {

  }

}

void setup() {
  // put your setup code here, to run once:
  
  //initialize serial monitor
  Serial.begin(9600);

  //initialize lcd
  lcd.begin(16, 2); //set up lcd's number of columns and rows
  lcd.setCursor(0,0);
  lcd.print("input: ");
  lcd.setCursor(11, 0);
  lcd.print('g');
  lcd.setCursor(0,1);
  lcd.print("filled: ");
  lcd.setCursor(12,1);
  lcd.print('g');
  lcd.setCursor(7, 0);
}

void loop() {
  // put your main code here, to run repeatedly:

  MeasurementInputSM_Tick();
  delay(200);
}
