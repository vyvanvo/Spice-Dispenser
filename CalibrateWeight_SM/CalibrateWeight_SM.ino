//Calibrate Weight SM

//HX711 library
#include <HX711.h>
//lcd library
#include <LiquidCrystal.h>

//initialize load cell HX711
HX711 scale;
const char dout = 2;
const char clk = 3;
unsigned long calibration_factor = 430;
float input_weight; //calibrated weight

//initialize lcd 
const char rs = 53, en = 51, d4 = 49, d5 = 47, d6 = 45, d7 = 43;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned char dispense_btn = 52;

enum CalibrateWeightSM_states {CW_Start, CW_Wait, CW_Calibrate} CW_state;

void CalibrateWeightSM_Tick(){

  bool done_dispense = (digitalRead(dispense_btn) == HIGH);
  Serial.println(done_dispense);
  
  //transtion
  switch(CW_state) {
    case CW_Start:
      CW_state = CW_Wait;
      break;

    case CW_Wait:
      if (!done_dispense) {
        CW_state = CW_Calibrate;
      }
      else {
        CW_state = CW_Wait;
      }

    case CW_Calibrate:
      if (!done_dispense) {
        CW_state = CW_Calibrate;
      }
      else {
        CW_state = CW_Wait;
      }
      break;

    default:
      CW_state = CW_Start;;
      break;
  }

  //action
  switch(CW_state) {
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
      
      Serial.print("Input/Calibrated weight: ");
      Serial.print(input_weight);
      Serial.println(" g");
      
      break;

    default:
      break;
  }
  
}

void setup() {
  // put your setup code here, to run once:

  //initialize serial monitor
  Serial.begin(9600);

  //initialize load cell scale
  scale.begin(dout, clk);
  scale.set_scale();
  scale.tare(); //set scale to 0;

  //initial btn
  pinMode(dispense_btn, INPUT_PULLUP);

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
}

unsigned long CW_prev_time = -200;
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long current_time = millis();
  if (current_time - CW_prev_time >= 200) {
    CalibrateWeightSM_Tick();
    CW_prev_time = current_time;
  }
  
}
