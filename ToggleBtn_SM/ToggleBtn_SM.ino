enum ledSM_states{
  START,
  OFF_RELEASE, 
  OFF_PRESS, 
  ON_RELEASE, 
  ON_PRESS
} led_state;

unsigned long period = 200;
//Output
int led_pin = 8;
//Input
int btn_pin = 2;

void Tick() {
  //read btn input
  bool btn_val = (digitalRead(btn_pin) == LOW); //pullup input
  
  switch(led_state) { //transitions
    case START:
      led_state = OFF_RELEASE;
      break;
      
    case OFF_RELEASE:
      if (btn_val) {
        led_state = ON_PRESS;
      }
      else {
        led_state = OFF_RELEASE;
      }
      break;

    case ON_PRESS:
      if (btn_val) {
        led_state = ON_PRESS;
      }
      else {
        led_state = ON_RELEASE;
      }
      break;

    case ON_RELEASE:
      if (btn_val) {
        led_state = OFF_PRESS;
      }
      else {
        led_state = ON_RELEASE;
      }
      break;

    case OFF_PRESS:
      if (btn_val) {
        led_state = OFF_PRESS;
      }
      else {
        led_state = OFF_RELEASE;
      }
      break;

    default:
    
      break;
      
  }

  switch(led_state) { //state actions
      case OFF_RELEASE:
          digitalWrite(led_pin, LOW);
        break;

      case ON_PRESS:
          digitalWrite(led_pin, HIGH);
        break;

      case ON_RELEASE:
          digitalWrite(led_pin, HIGH);
        break;

      case OFF_PRESS:
          digitalWrite(led_pin, LOW);
        break;

      default:
        break;
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(btn_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);
  led_state = START;
}

void loop() {
  // put your main code here, to run repeatedly:
  
  Tick();
  
  delay(period);
}
