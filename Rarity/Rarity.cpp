#include "Rarity.h"
#include "Arduino.h"
#include "Servo.h"
#include "Timers.h"

// TANK DRIVE
#define EN_LEFT          6   // Connected to E1 (Enable Pin) on L293
#define DIR_LEFT         7   // Connected to D1 (Direction Pin) on L293
#define EN_RIGHT         5   // Connected to E2 (Enable Pin) on L293
#define DIR_RIGHT        4  // Connected to D2 (Direction Pin) on L293
#define TURN_DURATION    100
#define FORWARD_DURATION 1000

// SHOOTER/LAUNCHER/TURRET/BRB
#define CAM_PIN          3 // Cam pin
#define TURRET_PIN       12 // servo
#define STRAIGHT         101
#define TWO_POINT        135
#define THREE_POINT      156 

// BALL REQUEST
#define REQUEST_PIN          13
#define REQUEST_ON           0
#define REQUEST_OFF          90
#define REQUEST_TIMER        2
#define REQUEST_DURATION_ON  500 // ms
#define REQUEST_DURATION_OFF 2650 // ms

// SERIAL
#define PRINT_TIMER      0
#define PRINT_DURATION   3000
#define MISC_TIMER       4
#define TWO_MINUTES      130000

Rarity::Rarity(void) {
  requesting = false;
  
  pinMode(CAM_PIN, OUTPUT);       // Cam
  
  pinMode(BACK_RIGHT, INPUT);                 
  pinMode(FRONT_RIGHT, INPUT);
  pinMode(FRONT_LEFT, INPUT);
  pinMode(BACK_LEFT, INPUT);
  
  pinMode(EN_LEFT, OUTPUT);      // sets digital pin 6 as output
  pinMode(DIR_LEFT, OUTPUT);         // sets digital pin 7 as output
  pinMode(EN_RIGHT, OUTPUT);      // sets digital pin 5 as output
  pinMode(DIR_RIGHT, OUTPUT);         // sets digital pin 4 as output
  
  state = FIRST_FORWARD;
  
  //TMRArd_InitTimer(PRINT_TIMER,PRINT_DURATION); 
  //Serial.println("Starting Constructor Rarity...");
}

Rarity::~Rarity(void) {
  //delete turret;
  //delete request;
}

void Rarity::initialize(void) {
  turret.attach(TURRET_PIN);
  turret.write(STRAIGHT);
  request.attach(REQUEST_PIN); 
  request.write(REQUEST_OFF);
  
  digitalWrite(EN_LEFT, LOW);
  digitalWrite(EN_RIGHT, LOW);
  digitalWrite(CAM_PIN, LOW);
  digitalWrite(DIR_LEFT, HIGH);      // Set L293 pin 7 as HIGH (Backward)
  digitalWrite(DIR_RIGHT, HIGH);      // Set L293 pin 4 as HIGH (Backward)
}

void Rarity::setDriveSpeed(int left, int right) {
  digitalWrite(DIR_LEFT, left < 0);
  digitalWrite(DIR_RIGHT, right < 0);
  analogWrite(EN_RIGHT, right < 0 ? -right : right);
  analogWrite(EN_LEFT, left < 0 ? -left : left);
}

void Rarity::setDriveMotor(char pin, int speed) {
  digitalWrite(pin, speed < 0);
  analogWrite(pin, speed < 0 ? -speed : speed);
}

boolean Rarity::isBumperHit(char bumper) {
  return digitalRead(bumper);
}

boolean Rarity::isReady(void) {
  if (TMRArd_GetTime() >= TWO_MINUTES) {
    setDriveSpeed(0,0);
    setShooterPower(0);
    state = GAME_OVER;
    return false;
  }
  return TMRArd_IsTimerExpired(STATE_TIMER) || !TMRArd_IsTimerActive(STATE_TIMER);
}

void Rarity::setShooterAngle(int angle) {
  turret.write(angle);
}

void Rarity::setShooterPower(int power) {
  analogWrite(CAM_PIN,power);
}

void Rarity::updateBallRequest(void) {
  if(TMRArd_IsTimerExpired(REQUEST_TIMER) || !TMRArd_IsTimerActive(REQUEST_TIMER)) {
    request.write(requesting ? REQUEST_OFF : REQUEST_ON);
    TMRArd_InitTimer(REQUEST_TIMER,requesting ? REQUEST_DURATION_OFF : REQUEST_DURATION_ON);
    requesting = !requesting;
  }
}

void Rarity::transitionToState(char next, int duration) {
  state = next;
  setDriveSpeed(0,0);
  TMRArd_InitTimer(STATE_TIMER,STATE_DELAY);
  if (duration > 0) TMRArd_InitTimer(MISC_TIMER,duration + STATE_DELAY);
}

char Rarity::getState(void) {
  return state;
}

void Rarity::setTimer(int duration) {
  TMRArd_InitTimer(MISC_TIMER,duration);
}

boolean Rarity::isTimerExpired(void) {
  return TMRArd_IsTimerExpired(MISC_TIMER);
}

