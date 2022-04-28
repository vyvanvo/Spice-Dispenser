//TASK SCHEDULER
typedef struct task {
  int state;
  unsigned long period;
  unsigned long prev_time;
  int (*TickFct)(int );
} task;

const unsigned char task_num = 1;
const unsigned long periodGCD = 100; //periodGCD
const unsigned long SS_period = 200;

task tasks[task_num]; //array of tasks


//SELECT SPICE VARIABLES

//use buttons as arrow keys to select a spice (indicated by leds)
//connect to led pins (output to show which spice is selected)
const int bled2 = 26, bled1 = 24, bled0 = 22;
unsigned char spice = 0x04;

//connect to button (arrow key inputs)
const int right_btn = 48, left_btn = 50;

//SELECT SPICE SM
enum SelectSpiceSM_states {SS_Start, SS_Wait, SS_Right, SS_Left} SS_state;

int SelectSpiceSM_Tick(int state) {
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

void setup() {
  // put your setup code here, to run once:

  //Serial monitor - (for debugging)
  Serial.begin(9600);
  
  //led outputs
  pinMode(bled2, OUTPUT);
  pinMode(bled1, OUTPUT);
  pinMode(bled0, OUTPUT);

  //btn inputs
  pinMode(right_btn, INPUT_PULLUP);
  pinMode(left_btn, INPUT_PULLUP);

  //initialize led outpus
  digitalWrite(bled2, HIGH);
  digitalWrite(bled1, LOW);
  digitalWrite(bled0, LOW);

  //initialze tasks
  tasks[0].state = SS_Start;
  tasks[0].period = SS_period;
  tasks[0].prev_time = -(tasks[0].prev_time); //set to negative of the period to allow task to execute during starting up ( millis() - (-taskperiod) => 0 + taskperiod = elapsedtime
  tasks[0].TickFct = &SelectSpiceSM_Tick;
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
