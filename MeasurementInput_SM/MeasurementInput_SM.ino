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
const char rs = 53, en = 51, d4 = 49, d5 = 47, d6 = 45, d7 = 43;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//initialize buttons
unsigned char reset_btn = 52;

//Measurement Input SM
//global variable
unsigned char dispense = 0;
unsigned long measurement = 0;

enum MeasurementInputSM_states {MI_Start, MI_Wait, MI_Reset, MI_PressKey, MI_EndofPress, MI_Backspace, MI_Enter} MI_state;

void MeasurementInputSM_Tick() {

  //get the character of the button pressed
  char key = keypad.getKey();
  // key = 0 --> no buttons pressed

  //get reset value
  bool reset = (digitalRead(reset_btn) == LOW); 

  static int keypad_inputs[3] = {0, 0, 0};
  static unsigned char k = 0; //iterator for keypad input array ({H, T, O})
  static unsigned char prev_k = 0;
  static unsigned char kp_input_places = 0;
  static unsigned char col_cursor = 7;
  static bool recently_backspaced = 0;
  
  //transition
  switch(MI_state) {
    case MI_Start:
      MI_state = MI_Wait;
      lcd.setCursor(7, 0);
      break;

    case MI_Wait:
      if (reset) {
        MI_state = MI_Reset;
        
        col_cursor = 7;
        lcd.setCursor(col_cursor, 0);
        lcd.print("   ");
        col_cursor = 7;
        measurement = 0;

        for (unsigned char i = 0; i < 3; i++) {
          keypad_inputs[i] = 0;
        }

        kp_input_places = 0;
        k = 0;

        //recently_backspaced = 0;
      }
      else {
        if (key == 0) {
          MI_state = MI_Wait;
        }
        else if (key == '*') {
          MI_state = MI_Backspace;

          if (col_cursor > 7) {
            
            recently_backspaced = 1;
            Serial.print("Backspace SM backspace: ");
            Serial.println(recently_backspaced);
            
            col_cursor--;
            lcd.setCursor(col_cursor, 0);
            lcd.print(' ');

            if (prev_k < k) {
              k--;
            }

            Serial.print("Backspace SM k (0 updated to): ");
            Serial.println(k, DEC);
            keypad_inputs[k] = 0;

            prev_k = k;

            if (k > 0) {
              k--;
            }

            Serial.print("Backspace SM k: ");
            Serial.println(k, DEC);

            if (kp_input_places > 0) {
              kp_input_places--;
              if (kp_input_places == 0) { //no number and reached the end -> cannot backspace anymore backspace
                recently_backspaced = 0;
              }
            }
            Serial.print("Backspace SM kp_input_places: ");
            Serial.println(kp_input_places, DEC);
          }
        
        }
        else if (key == '#') {
          MI_state = MI_Enter;
          dispense = 1;

          if (kp_input_places == 1) {
            measurement = keypad_inputs[0];
            Serial.println(measurement);
          }
          else if(kp_input_places == 2) {
            measurement = (keypad_inputs[0] * 10) + keypad_inputs[1];
            Serial.println(measurement);
          }
          else if (kp_input_places == 3) {
            measurement = (keypad_inputs[0] * 100) + (keypad_inputs[1] * 10) + (keypad_inputs[2]);
            Serial.println(measurement);
          }
          else {
            measurement = 0;
            Serial.println(measurement);
          }
   
        }
        else { //number key pressed
          if (col_cursor < 10) {
            MI_state = MI_PressKey;

            lcd.setCursor(col_cursor, 0);
            lcd.print(key);
            col_cursor++;

            if (recently_backspaced) { //recently backspaced so need to increment k to go to the next space, cannot replace the current number in the tens place
              k++;
            }

            prev_k = k; //set prev to current k, if prev_k == 1 and current k == 2, backspace by 1

            Serial.print("PressKey SM k (where key is updated to): ");
            Serial.println(k, DEC);
            
            keypad_inputs[k] = key - 48;
            //Serial.println(keypad_inputs[k], DEC);
            if (k < 2) {
              k++;
            }

            Serial.print("PressKey SM k (incremented k): ");
            Serial.println(k, DEC);

            if (kp_input_places < 3) {
              kp_input_places++;
            }
            //Serial.print(kp_input_places, HEX);

            recently_backspaced = 0;
            Serial.print("Press Key SM backspace: ");
            Serial.println(recently_backspaced);

          }
          else {
            MI_state = MI_EndofPress;
          }
        }
      }

      break;

    case MI_Reset:
      if (reset) {
        MI_state = MI_Reset;
      }
      else {
        MI_state = MI_Wait;
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

  //initialize reset btn
  pinMode(reset_btn, INPUT_PULLUP);

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
}

unsigned long MI_prev_time = -200; //set to -200 so that SM runs initially
//initial => millis() = 0
// 0 - (-200) = 200

void loop() {
  // put your main code here, to run repeatedly:

  unsigned long current_time = millis();
  if (current_time - MI_prev_time >= 200) { //check if elapsed time == 200
    MeasurementInputSM_Tick();
    MI_prev_time = current_time;
  }
    
}
