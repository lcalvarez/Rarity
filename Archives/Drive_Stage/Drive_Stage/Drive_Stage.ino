/**************************************************************
  File:     Drive_Stage.ino
  Contents:  

/*---------------- Includes ---------------------------------*/
#include <Timers.h>

/*---------------- Pin Defines ---------------------------*/
#define EnablePin_1     10   // Connected to E1 (Enable Pin) on L293
#define DirPin_1        11   // Connected to D1 (Direction Pin) on L293

#define EnablePin_2     5   // Connected to E2 (Enable Pin) on L293
#define DirPin_2        6  // Connected to D2 (Direction Pin) on L293

/*---------------- Time ----------------------------------*/
#define timer           0
#define timer_two       1
#define period          1 // in sec
#define period_turn     300 // in ticks (1000 ticks = 1 sec)
#define forward_time    1000 // in ticks

/*---------------- States ----------------------------------*/
#define BackingUp       10
#define Turning         11
#define DrivingStraight 12
#define Stop            13
#define SlightForward   14

#define threshold       500

/*---------------- Arduino Main Functions -------------------*/
void setup()
{
  Serial.begin(9600);
  Serial.println("Starting Arduino");
  pinMode(A5, INPUT);                // back left
  pinMode(A4, INPUT);                // back right
  
  pinMode(A1, INPUT);                // front left
  pinMode(A4, INPUT);                // front right
   
  pinMode(A0, INPUT);                // potentiometer reading
  pinMode(EnablePin_1, OUTPUT);      // sets digital pin 11 as output
  pinMode(DirPin_1, OUTPUT);         // sets digital pin 10 as output
  pinMode(EnablePin_2, OUTPUT);      // sets digital pin 5 as output
  pinMode(DirPin_2, OUTPUT);         // sets digital pin 6 as output
  digitalWrite(DirPin_1, HIGH);      // Set L293 pin 11 as HIGH (Backward)
  digitalWrite(DirPin_2, HIGH);      // Set L293 pin 6 as HIGH (Backward)
  TMRArd_InitTimer(1,3000);           
}

void loop(){
  
  // Keep track of time
  unsigned long total_time = TMRArd_GetTime();
 
  // Begin in the BackingUp state
  static unsigned char CurrentState = BackingUp;
  // Next state variable
  static unsigned char NextState = CurrentState;
 
  //--------- Bumpers ------------
 
  // back left bumper
  unsigned int front_left_bump = 0;
  front_left_bump = analogRead(A1);
 
  // back right bumper
  unsigned int front_right_bump = 0;
  front_right_bump = analogRead(A0);
 
  // back left bumper
  unsigned int back_left_bump = 0;
  back_left_bump = analogRead(A5);
 
  // back right bumper
  unsigned int back_right_bump = 0;
  back_right_bump = analogRead(A4);
 
 
//----------------------------
 
  // Speed
  unsigned int val_2 = 150;
 
  unsigned int pot = 0;
  pot = analogRead(A3)/5;
  if (pot > val_2) pot = val_2;
 
  unsigned int val_1 = val_2-pot;
 
  if (TMRArd_IsTimerExpired(1)) { // for output console
    TMRArd_InitTimer(1,3000);
    Serial.println("Front Right Bumper Reading");
    Serial.println(front_right_bump);
    Serial.println("Front Left Bumper Reading");
    Serial.println(front_left_bump);
    Serial.println("Back Right Bumper Reading");
    Serial.println(back_right_bump);
    Serial.println("Back Left Bumper Reading");
    Serial.println(back_left_bump);
    Serial.println("Current State is:");
    Serial.println(CurrentState);
    Serial.println("Pot value is");
    Serial.println(pot);
  }
   
  // Check current state of the bot
  switch(CurrentState) {
    case(BackingUp):
      Serial.println("Backing Up");
      // Set to backward at some speed
      digitalWrite(DirPin_1,HIGH);
      digitalWrite(DirPin_2,HIGH);
      // Send a PWM signal 
      analogWrite(EnablePin_1, val_1);
      analogWrite(EnablePin_2, val_2);
      // Check if back bumper has been hit
      if ((back_left_bump > threshold) && (back_right_bump > threshold)) { // if back_bump is at 5V then it's been hit
        Serial.println("Back Bump Hit");
        // Stop the motors
        analogWrite(EnablePin_1, 0);
        analogWrite(EnablePin_2, 0);
        // Assign the next state to be turning
        NextState = SlightForward;
        // Set the timer
        TMRArd_InitTimer(timer,forward_time);
      }
      break;
    case(SlightForward):
      Serial.println("Slight Forward");
      // Set one wheel to backward and one wheel forward
      digitalWrite(DirPin_1,LOW); // Left motor is forward
      digitalWrite(DirPin_2,LOW);  // Right motor is forward
      analogWrite(EnablePin_1, val_1);
      analogWrite(EnablePin_2, val_2);
      // Check if timer is expired
      if (TMRArd_IsTimerExpired(timer)) {
        Serial.println("Timer Expired");
        NextState = Turning;
        TMRArd_InitTimer(timer_two,period_turn);
      }
      break;
    case(Turning):
      Serial.println("Turning");
      // Set one wheel to backward and one wheel forward
      digitalWrite(DirPin_1,HIGH); // Left motor is forward
      digitalWrite(DirPin_2,LOW);  // Right motor is forward
      analogWrite(EnablePin_1, val_1);
      analogWrite(EnablePin_2, val_2);
      // Check if timer is expired
      if (TMRArd_IsTimerExpired(timer_two)) {
        Serial.println("Timer Expired");
        NextState = DrivingStraight;
      }
      break;
    case(DrivingStraight):
      Serial.println("Driving Straight");
      digitalWrite(DirPin_1,LOW); // Left motor is forward
      digitalWrite(DirPin_2,LOW);  // Right motor is forward
      analogWrite(EnablePin_1, val_1);
      analogWrite(EnablePin_2, val_2);
      if ((front_right_bump > threshold) && (front_right_bump > threshold)) { // if front_bump is at 5V then it's been hit
        Serial.println("Front Bump Hit");
        NextState = Stop;
      }
      break;
    case(Stop):
      analogWrite(EnablePin_1, 0);
      analogWrite(EnablePin_2, 0);
      break;
    default:
      break;
  }
  // Update the current state and return
  CurrentState = NextState;
}
  
