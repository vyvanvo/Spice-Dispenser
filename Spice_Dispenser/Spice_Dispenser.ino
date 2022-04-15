//included task scheduler (build up to have multiple tasks)

//spice selection

//use buttons as arrow keys to select a spice (indicated by leds)

//connect to led pins (output to show which spice is selected)
const int spice2 = 26, spice1 = 24, spice0 = 22;
unsigned char spice = 0x04; //global variable -> know which spice to dispense (data for Dispense SM)

//connect to button (arrow key inputs)
const int right_btn = 48, left_btn = 50;

//Task Scheduler
typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsed_time;
  int (*TickFct)(int );
} task;

const unsigned char task_num = 1;
const unsigned long periodGCD = 100; //periodGCD
const unsigned long SelectSpiceSM_period = 200;

task tasks[task_num]; //array of tasks

//SelectSppice SM
enum SelectSpiceSM_states {SS_Start, SS_Wait, SS_Right, SS_Left} SS_state;

int SelectSpiceSM_Tick(int state) {
  //get inputs
  bool right_btn_val = (digitalRead(right_btn) == LOW); //PULLUP BTN -> LOW = BTN PRESSED
  bool left_btn_val = (digitalRead(left_btn) == LOW); //PULLUP BTN -> LOW BTN_PRESSED
  //transition
  switch(state) {
    case SS_Start:
      SS_state = SS_Wait;
      spice = 0x04;
      break;

    case SS_Wait:
      if(right_btn_val && !left_btn_val) {
        SS_state = SS_Right;
        
        if (spice > 0x01) {
          spice = spice >> 1;
        }
      }
      else if (!right_btn_val && left_btn_val) {
        SS_state = SS_Left;

        if (spice < 0x04) {
          spice = spice << 1;
        }
      }
      else {
        SS_state = SS_Wait;
      }
      break;

    case SS_Right:
      if (right_btn_val) {
        SS_state = SS_Right;
      }
      else {
        SS_state = SS_Wait;
      }
      break;

    case SS_Left:  
      if (left_btn_val) {
        SS_state = SS_Left;
      }
      else {
        SS_state = SS_Wait;
      }
      break;

    default:
      SS_state = SS_Start;
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
    digitalWrite(spice2, HIGH);
    digitalWrite(spice1, LOW);
    digitalWrite(spice0, LOW);
  }
  else if (spice == 0x02) {
    digitalWrite(spice2, LOW);
    digitalWrite(spice1, HIGH);
    digitalWrite(spice0, LOW);
  }
  else if (spice == 0x01) {
    digitalWrite(spice2, LOW);
    digitalWrite(spice1, LOW);
    digitalWrite(spice0, HIGH);
  }

  return state;
}


void setup() {
  // put your setup code here, to run once:
  
  //led outputs
  pinMode(spice2, OUTPUT);
  pinMode(spice1, OUTPUT);
  pinMode(spice0, OUTPUT);

  //btn inputs
  pinMode(right_btn, INPUT_PULLUP);
  pinMode(left_btn, INPUT_PULLUP);

  //initialize led outpus
  digitalWrite(spice2, HIGH);
  digitalWrite(spice1, LOW);
  digitalWrite(spice0, LOW);

  //Serial monitor - (for debugging)
  Serial.begin(9600);

  //initialze tasks
  tasks[0].state = SS_Start;
  tasks[0].period = SelectSpiceSM_period;
  tasks[0].elapsed_time = tasks[0].period;
  tasks[0].TickFct = &SelectSpiceSM_Tick;
}


void loop() {
  // put your main code here, to run repeatedly:
  unsigned char i;
  for (i = 0; i < task_num; i++) {
    if (tasks[i].elapsed_time >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsed_time = 0;  
    }
    tasks[i].elapsed_time += periodGCD;
  }
  
  delay(periodGCD);
}
