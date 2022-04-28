//Dispense SM

//servo library
#include <Servo.h>

//green led output
unsigned char gled = 34;

//initialize servo
unsigned char servo_pin0 = 5;
unsigned char servo_pin1 = 6;
unsigned char servo_pin2 = 7;

Servo servo0;
Servo servo1;
Servo servo2;

const unsigned long open_pos = 135; //initial position
const unsigned long close_pos = 90; //final position

//test dispense with btn input
unsigned char dispense_btn = 52; 

//get input_weight with btn
//unsigned input_weight_btn;

//global variable
bool dispense = 0;
unsigned char spice = 0x01;
float input_weight; 
float target_weight = 10; 
bool done_dispense = 1;
unsigned long spice_amount[3] = {120, 120, 120}; //in grams

enum DispenseSM_states {D_Start, D_Wait, D_DispenseSpice0, D_DispenseSpice1, D_DispenseSpice2, D_GreenLED} D_state;

void DispenseSM_Tick() {
  //get dispense input
  bool dispense = (digitalRead(dispense_btn) == LOW);
  static unsigned char gled_counter = 0;

  //Serial.println(done_dispense);
  
  //transition
  switch(D_state) {
    case D_Start:
      D_state = D_Wait;
      break;

    case D_Wait:
      if (dispense && target_weight != 0) {
        if (spice == 0x01) {
          D_state = D_DispenseSpice0;
          
          servo0.write(open_pos);
          servo1.write(close_pos);
          servo2.write(close_pos);

          done_dispense = 0;
          
        }
        else if (spice == 0x02) {
          D_state = D_DispenseSpice1;
          
          servo0.write(close_pos);
          servo1.write(open_pos);
          servo2.write(close_pos);

          done_dispense = 0;
        }
        else if (spice == 0x04) {
          D_state = D_DispenseSpice2;

          servo0.write(close_pos);
          servo1.write(close_pos);
          servo2.write(open_pos);

          done_dispense = 0;
        }
        else {
          D_state = D_Wait;
        }
        
      }
      else {
        D_state = D_Wait;  
      }
      
      break;

    case D_DispenseSpice0:

      Serial.print("Spice 0: Input Weight? ");
      if (Serial.available()) {
        input_weight = Serial.parseInt();
      }
      else {
        input_weight = 0;
      }

      Serial.println(input_weight);

      //get input_weight
      if (input_weight >= target_weight) {
        spice_amount[0] = spice_amount[0] - input_weight;
        D_state = D_GreenLED;
        gled_counter = 0;

        for (unsigned char i = 0; i < 3; i++ ) {
          Serial.print(spice_amount[i]);
          Serial.print(' ');
        }

        Serial.println();
        
      }
      else {
        D_state = D_DispenseSpice0;
      }

      break;

    case D_DispenseSpice1:

      Serial.print("Spice 1: Input Weight? ");

      //get input weight
      if (Serial.available()) {
        input_weight = Serial.parseInt();
      }
      else {
        input_weight = 0;
      }

      Serial.println(input_weight);

      if (input_weight >= target_weight) {
        spice_amount[1] = spice_amount[1] - input_weight;
        D_state = D_GreenLED;
        gled_counter = 0;

        for (unsigned char i = 0; i < 3; i++ ) {
          Serial.print(spice_amount[i]);
          Serial.print(' ');
        }

        Serial.println();
      }
      else {
        D_state = D_DispenseSpice1;
      }

      break;

    case D_DispenseSpice2:

      Serial.print("Spice 2: Input Weight? ");

      //get input weight
      if (Serial.available()) {
        input_weight = Serial.parseInt();
      }
      else {
        input_weight = 0;
      }

      Serial.println(input_weight);


      if (input_weight >= target_weight) {
        spice_amount[2] = spice_amount[2] - input_weight;
        D_state = D_GreenLED;
        gled_counter = 0;

        for (unsigned char i = 0; i < 3; i++ ) {
          Serial.print(spice_amount[i]);
          Serial.print(' ');
        }

        Serial.println();
      }
      else {
        D_state = D_DispenseSpice2;
      }

      break;

    case D_GreenLED:
      
      if (gled_counter < 25) {
        D_state = D_GreenLED;
      }
      else {
        D_state = D_Wait;
      }
      break;

    default:
      D_state = D_Start;
      break;
      
  }

  //action
  switch(D_state) {

    case D_Wait:
      done_dispense = 1;
      digitalWrite(gled, LOW);
      
      servo0.write(close_pos);
      servo1.write(close_pos);
      servo2.write(close_pos);
      
      break;
    
    case D_GreenLED:
      servo0.write(close_pos);
      servo1.write(close_pos);
      servo2.write(close_pos);
    
      digitalWrite(gled, HIGH);
      gled_counter++;

      break;

    default:
      break;
  }
}

void setup() {
  // put your setup code here, to run once:

  //initialize serial monitor
  Serial.begin(9600);

  //btn input
  pinMode(dispense_btn, INPUT_PULLUP);

  //led outputs
  pinMode(gled, OUTPUT);

  //initialize green led
  digitalWrite(gled, LOW);

  //initialize servos
  servo0.attach(servo_pin0);
  servo1.attach(servo_pin1);
  servo2.attach(servo_pin2);

  servo0.write(close_pos);
  servo1.write(close_pos);
  servo2.write(close_pos);
}

unsigned long D_prev_time = -200;

void loop() {
  // put your main code here, to run repeatedly:

  unsigned char current_time = millis();

  if(current_time - D_prev_time >= 200) {
    DispenseSM_Tick();
    D_prev_time = current_time;
  }
  
}
