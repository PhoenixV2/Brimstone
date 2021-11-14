#include <MController.h>

#define MOTOR_IDLE 0
#define MOVETO 1
#define RUNSPEED 2

#define N_MOTORS 1

int EnablePin = 8;

int LED_pin = 13;
bool LED_status = true;

int xStep = 2;
int yStep = 3;
int zStep = 4;
int aStep = 12;
int xDir = 5;
int yDir = 6;
int zDir = 7;
int aDir = 13;
MController motor0(xDir, xStep);
MController motor1(yDir, yStep);
MController motor2(zDir, zStep);
MController motor3(aDir, aStep);
MController *motors[N_MOTORS];

float vList[1] = {500};
// For debugging
float target = 0;

int vdx = 0;
unsigned long prev_time = 0;
unsigned long cur_time;
int i = 0;

String Input;
char _buffer[32];
int command = MOTOR_IDLE;

void setup() {
  pinMode(EnablePin, OUTPUT);
  pinMode(LED_pin, OUTPUT);
  digitalWrite(EnablePin, LOW);
  
  motors[0] = &motor0;
//  motors[1] = &motor1;
//  motors[2] = &motor2;
//  motors[3] = &motor3;
  for(i = 0; i < N_MOTORS; i++){
    motors[i]->set_max_velocity(6000.0);
  }
  Serial.begin(57600);
  motor0.print_headers();
}

void loop() {
  if(Serial.available()){
    parseInput(); 
  }
  
//  for(i = 0; i < N_MOTORS; i++){
//    motors[i]->run_speed(vList[vdx]);
//    motors[i]->run_speed(target);
//  }
  runCommand();
  
//  if(motors[0]->get_velocity() == vList[vdx]){
//    vdx++;
//    vdx%=sizeof(vList)/sizeof(float);
//  }
  
  cur_time = millis();
  if(cur_time - prev_time >= 250){
    motors[0]->print_speed(target);
//    motors[0]->print_speed(vList[vdx]);
    prev_time = cur_time;
  }
}

void parseInput(){
void parseInput(){
  Input = Serial.readStringUntil('\n');
  Serial.println(Input);
  // Check if input length is longer than our buffer
  if(Input.length() > sizeof(_buffer)/sizeof(char)){
    Serial.println("Input message is too long");
    return;
  }
  // Convert to char[] to be able to be parsed with string.h <- I should probably continue to use strings
  Input.toCharArray(_buffer, Input.length()+1);

  // Expected format is <Action> <amount1 amount2 amount3 amount4>
  // number of amounts if 0..4 (I advise setting max value to the number of motors available)
  char *input;
  char *parsed[2];
  int i = 0;
  input = strtok(_buffer, " ,.[\n");
  while(input != NULL && i < 2){
    parsed[i] = input;
    input = strtok(NULL, "[]\n");
    i++;
  }
  char *action = parsed[0];

  // Parse the values sent after the action (can be none)
  char *curr_value;
  float values[N_MOTORS] = {0};
  int j = -1;
  if(parsed[1] != NULL && i>1){
    // i > 1 checks if there are more than 1 strings read into 'parsed'
    // first string is always action string followed by up to 4 numbers
    // if we don't do this check - the next strtok reads random garbage from
    // memory
    j = 0;
//    Serial.println(parsed[1]); // FOR DEBUGGING -> Prints entire line token
    curr_value = strtok(parsed[1], " [],\n");
    while(curr_value != NULL && j < N_MOTORS){
//      Serial.println(curr_value); // FOR DEBUGGING -> Prints each current value token
      values[j] = strtod(curr_value, NULL); // convert char* to float
      curr_value = strtok(NULL, " [],\n");  // get next char* token from the parsed input (parsed[1])
      j++;
    }
  }
  
  Serial.print("Action: ");
  Serial.print(action);
  if(j>0){
    Serial.print("\tValue: ");
    for(int k=0;k<j;k++){
      Serial.print(values[k]);
      Serial.print("\t");
    }
  }
  Serial.println();
  processCommands(action, values[0]);
}

void processCommands(char* action, float value){
//  target = value;
  if(strcmp(action, "moveTo") == 0){
    command = MOVETO;
    target = (int)value;
  }else if(strcmp(action, "runSpeed") == 0){
    command = RUNSPEED;
    target = value;
  }else{
    Serial.println("Invalid Command");
  }
}

void runCommand(){
  switch(command){
    case MOVETO:
      break;
    case RUNSPEED:
      for(int i = 0; i < N_MOTORS; i++){
        motors[i]->run_speed(target);
      }
      break;
    default:
      break;
  }
}
