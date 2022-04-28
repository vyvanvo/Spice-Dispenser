//LIBRARIES
//keypad library
#include <Keypad.h>
//lcd library
#include <LiquidCrystal.h>
//HX711 library
#include <HX711.h>
//servo library
#include <Servo.h>

//TASK SCHEDULER
typedef struct task {
  int state;
  unsigned long period;
  unsigned long prev_time;
  int (*TickFct)(int );
} task;

const unsigned char task_num = 5;
const unsigned long SS_period = 200;
const unsigned long MI_period = 200;
const unsigned long CW_period = 20;
const unsigned long D_period = 200;
const unsigned long SL_period = 200;

task tasks[task_num]; //array of tasks


//SELECT SPICE VARIABLES

//use buttons as arrow keys to select a spice (indicated by leds)
//connect to led pins (output to show which spice is selected)
const unsigned char bled2 = 26, bled1 = 24, bled0 = 22;

//global variable
unsigned char spice = 0x04;

//connect to button (arrow key inputs)
const char right_btn = 48, left_btn = 50;


//MEASUREMENT INPUT VARIABLES

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

//global variable
unsigned char dispense = 0;
int target_weight = 0;


//CALIBRATED WEIGHT VARIABLES
//initialize load cell HX711
HX711 scale;
const char dout = 2;
const char clk = 3;
const unsigned long calibration_factor = 430;

//global variable
float input_weight; //calibrated weight


//DISPENSE VARIABLES

//initialize servo
unsigned char servo_pin0 = 5;
unsigned char servo_pin1 = 6;
unsigned char servo_pin2 = 7;

Servo servo0;
Servo servo1;
Servo servo2;

const unsigned long open_pos2 = 82; //initial position
const unsigned long close_pos2 = 62; //final position

const unsigned long open_pos1 = 92; //initial position
const unsigned long close_pos1 = 72; //final position

const unsigned long open_pos0 = 95; //initial position
const unsigned long close_pos0 = 75; //final position
 
//get input_weight with btn
//unsigned input_weight_btn;

//global variable
bool done_dispense = 1;
int spice_amount[3] = {100, 100, 100}; //in grams 


//SPICE LOW VARIABLES
const unsigned char rled2 = 34, rled1 = 32, rled0 = 30;


//SELECT SPICE SM
enum SelectSpiceSM_states {SS_Start, SS_Wait, SS_Right, SS_Left} SS_state;

int SelectSpiceSM_Tick(int state) {
  //Serial.println("Select Spice SM");
  
  //get inputs
  bool right_btn_val = (digitalRead(right_btn) == LOW); //PULLUP BTN -> LOW = BTN PRESSED
  bool left_btn_val = (digitalRead(left_btn) == LOW); //PULLUP BTN -> LOW BTN_PRESSED
  //transition
  switch(state) {
    case SS_Start:
      state = SS_Wait;
      spice = 0x04;
      break;

    case SS_Wait:
      if(right_btn_val && !left_btn_val) {
        state = SS_Right;
        
        if (spice > 0x01) {
          spice = spice >> 1;
        }
      }
      else if (!right_btn_val && left_btn_val) {
        state = SS_Left;

        if (spice < 0x04) {
          spice = spice << 1;
        }
      }
      else {
        state = SS_Wait;
      }
      break;

    case SS_Right:
      if (right_btn_val) {
        state = SS_Right;
      }
      else {
        state = SS_Wait;
      }
      break;

    case SS_Left:  
      if (left_btn_val) {
        state = SS_Left;
      }
      else {
        state = SS_Wait;
      }
      break;

    default:
      state = SS_Start;
      break;
  
  }

  //action
  switch(state) {
    case SS_Wait:
      break;

    case SS_Right:
      break;

    case SS_Left:
      break;

    default:
      break;
  
  }

  if (spice == 0x04) {
    digitalWrite(bled2, HIGH);
    digitalWrite(bled1, LOW);
    digitalWrite(bled0, LOW);
  }
  else if (spice == 0x02) {
    digitalWrite(bled2, LOW);
    digitalWrite(bled1, HIGH);
    digitalWrite(bled0, LOW);
  }
  else if (spice == 0x01) {
    digitalWrite(bled2, LOW);
    digitalWrite(bled1, LOW);
    digitalWrite(bled0, HIGH);
  }

  return state;
}


//MEASUREMENT INPUT SM
enum MeasurementInputSM_states {MI_Start, MI_Wait, MI_Reset, MI_PressKey, MI_EndofPress, MI_Backspace, MI_Enter} MI_state;

int MeasurementInputSM_Tick(int state) {

  //Serial.println("Measurement Input SM");
  
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
  switch(state) {
    case MI_Start:
      state = MI_Wait;
      lcd.setCursor(7, 0);
      break;

    case MI_Wait:
      if (reset) {
        state = MI_Reset;
        
        lcd.setCursor(7, 0);
        lcd.print("   ");

        lcd.setCursor(8, 1);
        lcd.print("        ");

        lcd.setCursor(13, 1);
        lcd.print("g");
        
        col_cursor = 7;
        target_weight = 0;

        for (unsigned char i = 0; i < 3; i++) {
          keypad_inputs[i] = 0;
        }

        kp_input_places = 0;
        prev_k = 0;
        k = 0;

        //recently_backspaced = 0;
      }
      else {
        if (key == 0) {
          state = MI_Wait;
        }
        else if (key == '*') {
          state = MI_Backspace;

          if (col_cursor > 7) {
            
            recently_backspaced = 1;
            //Serial.print("Backspace SM backspace: ");
            //Serial.println(recently_backspaced);
            
            col_cursor--;
            lcd.setCursor(col_cursor, 0);
            lcd.print(' ');

            if (prev_k < k) {
              k--;
            }

            //Serial.print("Backspace SM k (0 updated to): ");
            //Serial.println(k, DEC);
            keypad_inputs[k] = 0;

            prev_k = k;

            if (k > 0) {
              k--;
            }

            //Serial.print("Backspace SM k: ");
            //Serial.println(k, DEC);

            if (kp_input_places > 0) {
              kp_input_places--;
              if (kp_input_places == 0) { //no number and reached the end -> cannot backspace anymore backspace
                recently_backspaced = 0;
              }
            }
            
            //Serial.print("Backspace SM kp_input_places: ");
            //Serial.println(kp_input_places, DEC);
            
          }
        
        }
        else if (key == '#') {
          state = MI_Enter;
          dispense = 1;

          if (kp_input_places == 1) {
            target_weight = keypad_inputs[0];
            Serial.print("input: ");
            Serial.println(target_weight);
          }
          else if(kp_input_places == 2) {
            target_weight = (keypad_inputs[0] * 10) + keypad_inputs[1];
            Serial.print("input: ");
            Serial.println(target_weight);
          }
          else if (kp_input_places == 3) {
            target_weight = (keypad_inputs[0] * 100) + (keypad_inputs[1] * 10) + (keypad_inputs[2]);
            Serial.print("input: ");
            Serial.println(target_weight);
          }
          else {
            target_weight = 0;
            Serial.print("input: ");
            Serial.println(target_weight);
          }
   
        }
        else { //number key pressed
          if (col_cursor < 10) {
            state = MI_PressKey;

            lcd.setCursor(col_cursor, 0);
            lcd.print(key);
            col_cursor++;

            if (recently_backspaced) { //recently backspaced so need to increment k to go to the next space, cannot replace the current number in the tens place
              k++;
            }

            prev_k = k; //set prev to current k, if prev_k == 1 and current k == 2, backspace by 1

            //Serial.print("PressKey SM k (where key is updated to): ");
            //Serial.println(k, DEC);
            
            keypad_inputs[k] = key - 48;
            //Serial.println(keypad_inputs[k], DEC);
            if (k < 2) {
              k++;
            }

            //Serial.print("PressKey SM k (incremented k): ");
            //Serial.println(k, DEC);

            if (kp_input_places < 3) {
              kp_input_places++;
            }
            //Serial.print(kp_input_places, HEX);

            recently_backspaced = 0;
            //Serial.print("Press Key SM backspace: ");
            //Serial.println(recently_backspaced);

          }
          else {
            state = MI_EndofPress;
          }
        }
      }

      break;

    case MI_Reset:
      if (reset) {
        state = MI_Reset;
      }
      else {
        state = MI_Wait;
      }
      break;

    case MI_PressKey:
      if (key == 0) {
        state = MI_Wait;
      }
      else {
        state = MI_PressKey;
      }
      break;

    case MI_EndofPress:
      if (key == 0) {
        state = MI_Wait;
      }
      else {
        state = MI_EndofPress;
      }
      break;

    case MI_Backspace:
      if (key == '*') {
        state = MI_Backspace;
      }
      else {
        state = MI_Wait;
      }
      break;

    case MI_Enter:
      if (!done_dispense) {
        state = MI_Enter;
      }
      else {
        state = MI_Wait;
        dispense = 0;
      }
      break;

    default:
      break;

  }

  //action
  switch(state) {
    case MI_Wait:
      break;
      
    case MI_Reset:
      break;
      
    case MI_PressKey:
      break;
      
    case MI_EndofPress:
      break;
      
    case MI_Backspace:
      break;
      
    case MI_Enter:
      break;

    default:
      break;
  }

  return state;
}


//CALIBRATED WEIGHT SM
enum CalibrateWeightSM_states {CW_Start, CW_Wait, CW_Calibrate} CW_state;

int CalibrateWeightSM_Tick(int state){

  //Serial.print("done_dispense: ");
  //Serial.println(done_dispense);

  //transtion
  switch(state) {
    case CW_Start:
      state = CW_Calibrate;
      break;

    case CW_Wait:
      if (!done_dispense) {
        state = CW_Calibrate;
        scale.set_scale();
        scale.tare();
      }
      else {
        state = CW_Wait;
        //scale.set_scale();
        //scale.tare();
      }s
      break;
      
    case CW_Calibrate:
      if (!done_dispense) {
        state = CW_Calibrate;
      }
      else {
        state = CW_Wait;
        //scale.set_scale();
        //scale.tare();
      }
      break;

    default:
      state = CW_Start;;
      break;
  }

  //action
  switch(state) {
    case CW_Wait:
      break;
    
    case CW_Calibrate:
      scale.set_scale(calibration_factor);
      input_weight = scale.get_units(10);
    
      if (input_weight < 0.0) {
        input_weight = 0.0;
      }

      if (input_weight < 10) {
        lcd.setCursor(12, 1);
        lcd.print(" g  ");
      }
      else if (input_weight < 100) {
        lcd.setCursor(13, 1);
        lcd.print(" g");
      }
      else if (input_weight < 1000) {
        lcd.setCursor(13, 1);
        lcd.print("  g");
      }

      lcd.setCursor(8,1); //row 2, column 9
      lcd.print(input_weight);
      
      //Serial.print("Input/Calibrated weight: ");
      //Serial.print(input_weight);
      //Serial.println(" g");
      
      break;

    default:
      break;
  }

  return state;
  
}


//DISPENSE SM
enum DispenseSM_states {D_Start, D_Wait, D_DispenseSpice0, D_DispenseSpice1, D_DispenseSpice2} D_state;

int DispenseSM_Tick(int state) {

  static unsigned char gled_counter = 0;

  //Serial.println(done_dispense);
  
  //transition
  switch(state) {
    case D_Start:
      state = D_Wait;
      break;

    case D_Wait:
      if (dispense && target_weight != 0) {
        if (spice == 0x01) {
          state = D_DispenseSpice0;
          
          servo0.write(open_pos0);
          servo1.write(close_pos1);
          servo2.write(close_pos2);

          done_dispense = 0;
          
        }
        else if (spice == 0x02) {
          state = D_DispenseSpice1;
          
          servo0.write(close_pos0);
          servo1.write(open_pos1);
          servo2.write(close_pos2);

          done_dispense = 0;
        }
        else if (spice == 0x04) {
          state = D_DispenseSpice2;

          servo0.write(close_pos0);
          servo1.write(close_pos1);
          servo2.write(open_pos2);

          done_dispense = 0;
        }
        else {
          state = D_Wait;
        }
        
      }
      else {
        state = D_Wait;  
      }
      
      break;

    case D_DispenseSpice0:

      Serial.print("Spice 0 Input Weight: ");
      Serial.println(input_weight);

      //get input_weight
      if (input_weight >= (target_weight - 1)) {
        spice_amount[0] = spice_amount[0] - input_weight;
        state = D_Wait;

        servo0.write(close_pos0);
        servo1.write(close_pos1);
        servo2.write(close_pos2);

        for (unsigned char i = 0; i < 3; i++ ) {
          Serial.print(spice_amount[i]);
          Serial.print(' ');
        }

        Serial.println();
        
      }
      else {
        state = D_DispenseSpice0;
      }

      break;

    case D_DispenseSpice1:

      Serial.print("Spice 1 Input Weight: ");
      Serial.println(input_weight);

      if (input_weight >= (target_weight - 1)) {
        spice_amount[1] = spice_amount[1] - input_weight;

        state = D_Wait;
        //gled_counter = 0;

        servo0.write(close_pos0);
        servo1.write(close_pos1);
        servo2.write(close_pos2);
        
        for (unsigned char i = 0; i < 3; i++ ) {
          Serial.print(spice_amount[i]);
          Serial.print(' ');
        }

        Serial.println();
      }
      else {
        state = D_DispenseSpice1;
      }

      break;

    case D_DispenseSpice2:

      Serial.print("Spice 2 Input Weight: ");
      Serial.println(input_weight);


      if (input_weight >= (target_weight - 1)) {
        spice_amount[2] = spice_amount[2] - input_weight;
        state = D_Wait;
        //gled_counter = 0;

        servo0.write(close_pos0);
        servo1.write(close_pos1);
        servo2.write(close_pos2);
        
        for (unsigned char i = 0; i < 3; i++ ) {
          Serial.print(spice_amount[i]);
          Serial.print(' ');
        }

        Serial.println();
      }
      else {
        state = D_DispenseSpice2;
      }

      break;
      
    default:
      state = D_Start;
      break;
      
  }

  //action
  switch(state) {

    case D_Wait:
      done_dispense = 1;
      break;

    default:
      break;
  }

  return state;
}


//SPICE LOW SM
enum SpiceLowSM_states {SL_Start, SL_LED} SL_state;

int SpiceLowSM_Tick(int state) {

  //transition
  switch(state) {
    case SL_Start:
      state = SL_LED;
      break;

    case SL_LED:
      state = SL_LED;
      break;

    default:
      state = SL_LED;
      break;
  }
  
  //action
  switch(state) {
    case SL_LED:

      digitalWrite(rled2, LOW);
      digitalWrite(rled1, LOW);
      digitalWrite(rled0, LOW);

      if (spice_amount[0] < 50) {
        digitalWrite(rled0, HIGH);
      }

      if (spice_amount[1] < 50) {
        digitalWrite(rled1, HIGH);
      }

      if (spice_amount[2] < 50) {
        digitalWrite(rled2, HIGH);
      }
      
      break;

    default:
      break;
  }
  return state;
}

void setup() {
  // put your setup code here, to run once:

  //Serial monitor - (for debugging)
  Serial.begin(9600);
  
  //initialize blue led outputs
  pinMode(bled2, OUTPUT);
  pinMode(bled1, OUTPUT);
  pinMode(bled0, OUTPUT);

  digitalWrite(bled2, HIGH);
  digitalWrite(bled1, LOW);
  digitalWrite(bled0, LOW);

  //initialize blue led outputs
  pinMode(rled2, OUTPUT);
  pinMode(rled1, OUTPUT);
  pinMode(rled0, OUTPUT);

  digitalWrite(rled2, LOW);
  digitalWrite(rled1, LOW);
  digitalWrite(rled0, LOW);

  //btn inputs
  pinMode(right_btn, INPUT_PULLUP);
  pinMode(left_btn, INPUT_PULLUP);

  //initialize reset btn
  pinMode(reset_btn, INPUT_PULLUP);

  //initialize load cell scale
  scale.begin(dout, clk);
  scale.set_scale();
  scale.tare(); //set scale to 0;

  //initialize servos
  servo0.attach(servo_pin0);
  servo1.attach(servo_pin1);
  servo2.attach(servo_pin2);

  servo0.write(close_pos0);
  servo1.write(close_pos1);
  servo2.write(close_pos2);

  //initialize lcd
  lcd.begin(16, 2); //set up lcd's number of columns and rows
  lcd.setCursor(0,0);
  lcd.print("input: ");
  lcd.setCursor(11, 0);
  lcd.print('g');
  lcd.setCursor(0,1);
  lcd.print("filled: ");
  lcd.setCursor(13,1);
  lcd.print('g');
  
  //initialze tasks
  tasks[0].state = SS_Start;
  tasks[0].period = SS_period;
  tasks[0].prev_time = -(tasks[0].prev_time); //set to negative of the period to allow task to execute during starting up ( millis() - (-taskperiod) => 0 + taskperiod = elapsedtime
  tasks[0].TickFct = &SelectSpiceSM_Tick;

  tasks[1].state = MI_Start;
  tasks[1].period = MI_period;
  tasks[1].prev_time = -(tasks[1].prev_time); //set to negative of the period to allow task to execute during starting up ( millis() - (-taskperiod) => 0 + taskperiod = elapsedtime
  tasks[1].TickFct = &MeasurementInputSM_Tick;

  tasks[2].state = CW_Start;
  tasks[2].period = CW_period;
  tasks[2].prev_time = -(tasks[2].prev_time); //set to negative of the period to allow task to execute during starting up ( millis() - (-taskperiod) => 0 + taskperiod = elapsedtime
  tasks[2].TickFct = &CalibrateWeightSM_Tick;

  tasks[3].state = D_Start;
  tasks[3].period = D_period;
  tasks[3].prev_time = -(tasks[3].prev_time); //set to negative of the period to allow task to execute during starting up ( millis() - (-taskperiod) => 0 + taskperiod = elapsedtime
  tasks[3].TickFct = &DispenseSM_Tick;

  tasks[4].state = SL_Start;
  tasks[4].period = SL_period;
  tasks[4].prev_time = -(tasks[4].prev_time); //set to negative of the period to allow task to execute during starting up ( millis() - (-taskperiod) => 0 + taskperiod = elapsedtime
  tasks[4].TickFct = &SpiceLowSM_Tick;
}


void loop() {
  // put your main code here, to run repeatedly:
  unsigned char i;
  unsigned long current_time;
  for (i = 0; i < task_num; i++) {
    current_time = millis();
    if ( (current_time - tasks[i].prev_time) >= tasks[i].period ) { //check elasped time > period
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].prev_time = current_time;  //set previous time to current time
    }
  }
}
