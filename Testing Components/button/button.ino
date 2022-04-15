//pull up buttons

//input buttons
short btn1 = 23;
short btn2 = 25;
short btn3 = 27;
short btn4 = 29;
short btn5 = 31;
short btn6 = 33;
short btn7 = 35;

//leds
short led1 = 41;
short led2 = 43;
short led3 = 45;
short led4 = 47;
short led5 = 49;
short led6 = 51;
short led7 = 53;

//read input variable
char btn1_val;
char btn2_val;
char btn3_val;
char btn4_val;
char btn5_val;
char btn6_val;
char btn7_val;

void setup() {
  // put your setup code here, to run once:b

  //inputs - pull up btns
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);
  pinMode(btn5, INPUT_PULLUP);
  pinMode(btn6, INPUT_PULLUP);
  pinMode(btn7, INPUT_PULLUP);

  //outputs
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);

  //initalize to all off
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(led6, LOW);
  digitalWrite(led7, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  //read inputs
  //pull up input -> negate
  btn1_val = digitalRead(btn1);
  btn2_val = digitalRead(btn2);
  btn3_val = digitalRead(btn3);
  btn4_val = digitalRead(btn4);
  btn5_val = digitalRead(btn5);
  btn6_val = digitalRead(btn6);
  btn7_val = digitalRead(btn7);

  if (btn1_val == 0) {
    digitalWrite(led1, HIGH);
  }
  else {
    digitalWrite(led1, LOW);
  }

  if (btn2_val == 0) {
    digitalWrite(led2, HIGH);
  }
  else {
    digitalWrite(led2, LOW);
  }

  if (btn3_val == 0) {
    digitalWrite(led3, HIGH);
  }
  else {
    digitalWrite(led3, LOW);
  }

  if (btn4_val == 0) {
    digitalWrite(led4, HIGH);
  }
  else {
    digitalWrite(led4, LOW);
  }

  if (btn5_val == 0) {
    digitalWrite(led5, HIGH);
  }
  else {
    digitalWrite(led5, LOW);
  }

  if (btn6_val == 0) {
    digitalWrite(led6, HIGH);
  }
  else {
    digitalWrite(led6, LOW);
  }

  if (btn7_val == 0) {
    digitalWrite(led7, HIGH);
  }
  else {
    digitalWrite(led7, LOW);
  }
  
}
