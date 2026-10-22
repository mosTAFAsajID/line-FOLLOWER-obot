#include <QTRSensors.h>

// ---------------- Motor Pins ----------------
#define PWMA 10
#define AIN1 12
#define AIN2 13

#define PWMB 11
#define BIN1 A0
#define BIN2 A1

#define BUZZER A2
#define LEDON A3

// ---------------- Speeds ----------------
#define STRAIGHT_SPEED 80
#define TURN_SPEED 120
#define SHARP_TURN_SPEED 150
#define PIVOT_SPEED 170
#define MAX_SPEED 200

// ---------------- Sensors ----------------
QTRSensors qtr;
const uint8_t SENSOR_COUNT = 8;
uint16_t sensorValues[SENSOR_COUNT];

int lastDirection = 1;   // -1 = left, +1 = right

// ------------------------------------------------

void setup() {
  Serial.begin(9600);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(BUZZER, OUTPUT);
  pinMode(LEDON, OUTPUT);

  digitalWrite(LEDON, HIGH);

  qtr.setTypeRC();

  // Your sensor array is mounted backwards
  qtr.setSensorPins((const uint8_t[]){9,8,7,6,5,4,3,2}, SENSOR_COUNT);

  tone(BUZZER,1200,200);
  delay(300);

  for(int i=0;i<400;i++){
    qtr.calibrate();
    delay(10);
  }

  tone(BUZZER,1800,200);
  delay(300);
}

// ------------------------------------------------

void moveForward(int left,int right){

  left = constrain(left,0,MAX_SPEED);
  right = constrain(right,0,MAX_SPEED);

  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  analogWrite(PWMA,left);

  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
  analogWrite(PWMB,right);
}

// ------------------------------------------------

bool lineVisible(){

  for(int i=0;i<SENSOR_COUNT;i++){
    if(sensorValues[i]>200)
      return true;
  }

  return false;
}

// ------------------------------------------------

void recover(){

  if(lastDirection<0)
    moveForward(0,PIVOT_SPEED);
  else
    moveForward(PIVOT_SPEED,0);
}

// ------------------------------------------------

void loop(){

  uint16_t position = qtr.readLineBlack(sensorValues);

  if(!lineVisible()){
    recover();
    return;
  }

  // Remember last direction
  if(position<3500)
    lastDirection=-1;
  else
    lastDirection=1;

  // -------- Bang-Bang Zones --------

  if(position<700){              // Extreme Left
    moveForward(0,PIVOT_SPEED);
  }

  else if(position<1800){        // Left
    moveForward(40,SHARP_TURN_SPEED);
  }

  else if(position<2800){        // Slight Left
    moveForward(70,TURN_SPEED);
  }

  else if(position<4200){        // Center
    moveForward(STRAIGHT_SPEED,STRAIGHT_SPEED);
  }

  else if(position<5200){        // Slight Right
    moveForward(TURN_SPEED,70);
  }

  else if(position<6300){        // Right
    moveForward(SHARP_TURN_SPEED,40);
  }

  else{                          // Extreme Right
    moveForward(PIVOT_SPEED,0);
  }

  Serial.print(position);
  Serial.print('\t');
  Serial.print(sensorValues[0]);
  Serial.print('\t');
  Serial.println(sensorValues[7]);

  delay(2);
}