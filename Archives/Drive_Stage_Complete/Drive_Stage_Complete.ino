/**************************************************************
  File:     Drive_Stage.ino
  Contents:  

/*---------------- Includes ---------------------------------*/
#include <Timers.h>

/*---------------- Pin Defines ---------------------------*/
#define EnablePin_1     6   // Connected to E1 (Enable Pin) on L293
#define DirPin_1        7   // Connected to D1 (Direction Pin) on L293

#define EnablePin_2     5   // Connected to E2 (Enable Pin) on L293
#define DirPin_2        4  // Connected to D2 (Direction Pin) on L293

/*---------------- Time ----------------------------------*/
#define timer_one       0
#define timer_two       1
#define stall_timer     2

#define console_timer   1

#define period          1 // in sec
#define period_turn     300 // in ticks (1000 ticks = 1 sec)
#define forward_time    400 // in ticks
#define stall_time      2000

#define divisor         3 // divisor for rotating wheel (how much to divide by for the left wheel to rotate)

/*---------------- States ----------------------------------*/
#define FirstForward    9
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
  
  pinMode(A1, INPUT);                // front right
  pinMode(A0, INPUT);                // back right
  
  pinMode(A4, INPUT);                // back left
  pinMode(A5, INPUT);                // front left
  
   
  pinMode(A2, INPUT);                // 
  pinMode(A3, INPUT);
  
  pinMode(EnablePin_1, OUTPUT);      // sets digital pin 6 as output
  pinMode(DirPin_1, OUTPUT);         // sets digital pin 7 as output
  pinMode(EnablePin_2, OUTPUT);      // sets digital pin 5 as output
  pinMode(DirPin_2, OUTPUT);         // sets digital pin 4 as output
  
  digitalWrite(DirPin_1, HIGH);      // Set L293 pin 7 as HIGH (Backward)
  digitalWrite(DirPin_2, HIGH);      // Set L293 pin 4 as HIGH (Backward)
  
  TMRArd_InitTimer(console_timer,3000);           
}

void loop(){
  
  // Keep track of time
  unsigned long total_time = TMRArd_GetTime();
 
  // Begin in the BackingUp state
  static unsigned char CurrentState = FirstForward;
  // Next state variable
  static unsigned char NextState = CurrentState;
 
  //--------- Bumpers ------------
 
  // back left bumper
  unsigned int front_left_bump = 0;
  front_left_bump = analogRead(A5);
 
  // back right bumper
  unsigned int front_right_bump = 0;
  front_right_bump = analogRead(A1);
 
  // back left bumper
  unsigned int back_left_bump = 0;
  back_left_bump = analogRead(A4);
 
  // back right bumper
  unsigned int back_right_bump = 0;
  back_right_bump = analogRead(A0);
 
 
  //----------------------------
 
  // Speed
  unsigned int val_2 = 115;
 
  unsigned int pot = 0;
  pot = analogRead(A3)/5;
  if (pot > val_2) pot = val_2;
 
  unsigned int val_1 = val_2;
 
  if (TMRArd_IsTimerExpired(console_timer)) { // for output console
    TMRArd_InitTimer(console_timer,3000);
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
    case(FirstForward):
      // Set to backward at some speed
      digitalWrite(DirPin_1,LOW);
      digitalWrite(DirPin_2,LOW);
      // Send a PWM signal with one offset to turn into the wall
      analogWrite(EnablePin_1, val_1 - 15);
      analogWrite(EnablePin_2, val_2);
      if ((front_left_bump > threshold) && (front_right_bump < threshold)) { // if only one side is hit, stop that wheel on that side and drive hard on the other
        analogWrite(EnablePin_1, 0);
        analogWrite(EnablePin_2, val_2 + 50);
      }
      if ((front_left_bump < threshold) && (front_right_bump > threshold)) { // if only one side is hit, stop that wheel on that side
        analogWrite(EnablePin_2, 0);
        analogWrite(EnablePin_1, val_1 + 50);
      }
      // Check if back bumper has been hit
      if ((front_left_bump > threshold) && (front_right_bump > threshold)) { // if front_bump is at 5V then it's been hit
        Serial.println("Front Bump Hit");
        // Stop the motors
        analogWrite(EnablePin_1, 0);
        analogWrite(EnablePin_2, 0);
        // Assign the next state to be turning
        NextState = BackingUp;
        TMRArd_InitTimer(stall_timer,stall_time);
      }
    break;
    case(BackingUp):
      Serial.println("Backing Up");
      if (TMRArd_IsTimerExpired(stall_timer)) {
        // Set to backward at some speed
        digitalWrite(DirPin_1,HIGH);
        digitalWrite(DirPin_2,HIGH);
        // Send a PWM signal with one offset to turn into the wall
        analogWrite(EnablePin_1, val_1 - 15);
        analogWrite(EnablePin_2, val_2);
        if ((back_left_bump > threshold) && (back_right_bump < threshold)) { // if only one side is hit, stop that wheel on that side and drive hard on the other
          analogWrite(EnablePin_1, 0);
          analogWrite(EnablePin_2, val_2 + 50);
        }
        if ((back_left_bump < threshold) && (back_right_bump > threshold)) { // if only one side is hit, stop that wheel on that side
          analogWrite(EnablePin_2, 0);
          analogWrite(EnablePin_1, val_1 + 50);
        }
        // Check if back bumper has been hit
        if ((back_left_bump > threshold) && (back_right_bump > threshold)) { // if back_bump is at 5V then it's been hit
          Serial.println("Back Bump Hit");
          // Stop the motors
          analogWrite(EnablePin_1, 0);
          analogWrite(EnablePin_2, 0);
          // Assign the next state to be turning
          NextState = SlightForward;
          // Set the timer
          TMRArd_InitTimer(stall_timer,stall_time);
          TMRArd_InitTimer(timer_one,stall_time + forward_time);
        }
      }
      break;
    case(SlightForward):
      Serial.println("Slight Forward");
      // Check that stall timer is done and then continue
      if (TMRArd_IsTimerExpired(stall_timer)) {
        // Set one wheel to backward and one wheel forward
        digitalWrite(DirPin_1,LOW); // Left motor is forward
        digitalWrite(DirPin_2,LOW);  // Right motor is forward
        analogWrite(EnablePin_1, val_1);
        analogWrite(EnablePin_2, val_2);
        // Check if timer is expired
        if (TMRArd_IsTimerExpired(timer_one)) {
          Serial.println("Timer Expired");
          analogWrite(EnablePin_1, 0); // turn off momentarily
          analogWrite(EnablePin_2, 0); // turn off momentarily
          NextState = Turning;
          TMRArd_InitTimer(stall_timer,stall_time);
          TMRArd_InitTimer(timer_two,stall_time + period_turn);
        }
      }
      break;
    case(Turning):
      Serial.println("Turning");
      if (TMRArd_IsTimerExpired(stall_timer)) {
        // Set one wheel to backward and one wheel forward
        digitalWrite(DirPin_1,HIGH); // Left motor is backward
        digitalWrite(DirPin_2,HIGH);  // Right motor is forward
        analogWrite(EnablePin_1, val_1); // offset to rotate inward
        analogWrite(EnablePin_2, val_2 - 100);
        // Check if timer is expired
        if (TMRArd_IsTimerExpired(timer_two)) {
          analogWrite(EnablePin_1, 0);  // turn off momentarily
          analogWrite(EnablePin_2, 0);  // turn off moentarrily
          Serial.println("Timer Expired");
          NextState = DrivingStraight;
          TMRArd_InitTimer(stall_timer,stall_time);
        }
      }
      break;
    case(DrivingStraight):
      if (TMRArd_IsTimerExpired(stall_timer)) {
        Serial.println("Driving Straight");
        digitalWrite(DirPin_1,LOW); // Left motor is forward
        digitalWrite(DirPin_2,LOW);  // Right motor is forward
        analogWrite(EnablePin_1, val_1 - 10); // go slower here
        analogWrite(EnablePin_2, val_2 + 10); // go slower here but run into the wall
        if ((front_right_bump > threshold) && (front_left_bump > threshold)) { // if front_bump is at 5V then it's been hit
          Serial.println("Front Bump Hit");
          NextState = Stop;
        }
      }
      break;
    case(Stop):
      analogWrite(EnablePin_1, 0); // low PWM to keep against wall
      analogWrite(EnablePin_2, 0); // low PWM to keep against wall
      break;
    default:
      break;
  }
  // Update the current state and return
  CurrentState = NextState;
}
