//Servo
#include <Servo.h>

short servo_pin1 = 23;
short servo_pin2 = 25;
short servo_pin3 = 27;

int servo_pos = 180;
Servo servo1;
Servo servo2;
Servo servo3;

//servo 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo1.attach(servo_pin1);
  servo2.attach(servo_pin2);
  servo3.attach(servo_pin3);
}

void loop() {
  // put your main code here, to run repeatedly:

  // changing angle of servo
  //wait for input in the monitor
  Serial.println("What angle for the Servo?");
  while (!Serial.available()) {

  }

  servo_pos = Serial.parseInt();

  Serial.print("Angle: ");
  Serial.println(servo_pos);
  
  servo1.write(servo_pos); //change postion of servo
  servo2.write(servo_pos);
  servo3.write(servo_pos);
}
