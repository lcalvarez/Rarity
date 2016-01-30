/**************************************************************
  File:     Base_Motors.ino
  Contents: This program demonstrates the use of the software
            for driving the base motors. By connecting the
            Arduino to the L293 Driver, the program is able 
            to turn each wheel clockwise or counter-clockwise
            such that the bot has full planar DOF. 
            
            In this configuration, LOW corresponds to Forward
            and HIGH corresponds to Backward. This can be 
            reversed if one reverses the moterconnections in 
            the area J1 of the L293 module. 

/*---------------- Includes ---------------------------------*/
#include <Timers.h>

/*---------------- Pin Defines ---------------------------*/
#define EnablePin_1     10   // Connected to E1 (Enable Pin) on L293
#define DirPin_1        11   // Connected to D1 (Direction Pin) on L293

#define EnablePin_2     5   // Connected to E2 (Enable Pin) on L293
#define DirPin_2        6  // Connected to D2 (Direction Pin) on L293

/*---------------- Time ----------------------------------*/
#define timer           0
#define period          1 // in sec

/*---------------- Arduino Main Functions -------------------*/
void setup()
{
  Serial.begin(9600);
  Serial.println("Starting Arduino. Part 4.");
  pinMode(A5, INPUT);
  pinMode(A0, INPUT);
  pinMode(EnablePin_1, OUTPUT);      // sets digital pin 11 as output
  pinMode(DirPin_1, OUTPUT);         // sets digital pin 10 as output
  pinMode(EnablePin_2, OUTPUT);      // sets digital pin 5 as output
  pinMode(DirPin_2, OUTPUT);         // sets digital pin 6 as output
  digitalWrite(DirPin_1, LOW);       // Set L293 pin 11 as LOW (Forward)
  digitalWrite(DirPin_2, LOW);       // Set L293 pin 6 as LOW (Forward)
  TMRArd_InitTimer(timer,period*1000); // Timer for output to console
}

void loop(){
  // Keep track of time
  unsigned long total_time = TMRArd_GetTime();
  
  // Change direction if a key was pressed. 
  unsigned char KeyEventOccurred = 0;
  KeyEventOccurred = Serial.available();
  char input = Serial.read();
  if (KeyEventOccurred) {
    if (digitalRead(DirPin_1) & digitalRead(DirPin_2)) { // if both high, set to low
      digitalWrite(DirPin_1,LOW);
      digitalWrite(DirPin_2,LOW);
      Serial.println("Changed to low");
    } else {                                             // if both low, set to high
      digitalWrite(DirPin_1,HIGH);
      digitalWrite(DirPin_2,HIGH);
      Serial.println("Changed to high");
    }
  }
  
  // Read in the voltage from the potentiometer
  unsigned int val = 0;
  val = analogRead(A0);
  // Floor to 255 if val is greater than 255 for resolution
  if (val > 255){
    val = 255;
  }
  
  
  // Read in the voltage from the front bumper 
  unsigned int front_bump = 0;
  front_bump = analogRead(A5);
  
  if (front_bump > 100) { // front bump was hit
    // Send a null PWM signal 
    analogWrite(EnablePin_1, 0);
    analogWrite(EnablePin_2, 0);
  } else {
    // Send a PWM signal 
    analogWrite(EnablePin_1, 150);
    analogWrite(EnablePin_2, 200);
  }
  
  // If the timer expires, output to the screen
  if (TMRArd_IsTimerExpired(timer)) {
    TMRArd_InitTimer(timer,period*1000);
    Serial.println("Left Motor is in State:");
    Serial.println(digitalRead(DirPin_1));
    Serial.println("Right Motor is in State:");
    Serial.println(digitalRead(DirPin_2));
    Serial.println("Front Bumper is showing");
    Serial.println(front_bump);
  }
}
