#include <MController.h>
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

float vList[6] = {3000, -3000, 0, -500, 500, 0};//, 100, 250, 1000, 0}; 
int vdx = 0;
unsigned long prev_time = 0;
unsigned long cur_time;
int i = 0;

void setup() {
  pinMode(EnablePin, OUTPUT);
  pinMode(LED_pin, OUTPUT);
  digitalWrite(EnablePin, LOW);
  // put your setup code here, to run once:
  motors[0] = &motor0;
//  motors[1] = &motor1;
//  motors[2] = &motor2;
//  motors[3] = &motor3;
  for(i = 0; i < N_MOTORS; i++){
    motors[i]->set_max_velocity(3000.0);
//    motors[i]->set_speed(500);  
  }
  Serial.begin(57600);
  motor0.print_headers();
}

void loop() {
  // put your main code here, to run repeatedly:
  for(i = 0; i < N_MOTORS; i++){
    motors[i]->calc_new_velocity(vList[vdx]);
    motors[i]->calc_interval();
    motors[i]->run_velocity();  
  }
  if(motors[0]->get_velocity() == vList[vdx]){
    vdx++;
    vdx%=sizeof(vList)/sizeof(float);
  }
  
  cur_time = millis();
  if(cur_time - prev_time >= 250){
    motors[0]->print_speed(vList[vdx]);
    prev_time = cur_time;
  }
}
