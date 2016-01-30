/**************************************************************
 File:     Stepper_Motor.ino
 Contents: This program demonstrates the use of the software
           for driving the stepper motor using the Pulse
           library provided by the ME 210 staff. After 
           setting up an initial timer, the program reads
           in a voltage from a potentiometer and sends
           that value as a pulse in a predefined period.
           After a particular time has passed, the stepper
           will then rotate in the opposite direction.

/*---------------- Headers---- ---------------------------*/
#include <Pulse.h>
#include <Timers.h>

/*---------------- Pin Defines ---------------------------*/
#define StepPin    6
#define DirPin     5
#define Low        0
#define High       1

/*---------------- Time -----------------------------------*/
#define whole_time 3      // In seconds
#define period     10     // 1/10 of ms
#define ticks      1000   // 1000 ticks = 1 sec

/*---------------- Arduino Main Functions -------------------*/

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Arduino.");
  pinMode(A0, INPUT);           // sets the analog pin as input
  pinMode(DirPin, OUTPUT);      // sets the digital pin as output
  // Setup initial timer
  TMRArd_InitTimer(0,ticks);    // set timer
  InitPulse(StepPin, period);   // Initialize the pulse
}

void loop() {
  // Read in if keyboard is pressed
  unsigned char KeyEventOccurred = 0;
  KeyEventOccurred = Serial.available();
  static unsigned int state = 0;
  char input = Serial.read();
  // Change the direction if the keyboard is pressed
  if (KeyEventOccurred) {
    if (state == 0){
      digitalWrite(DirPin,HIGH);   // Change to High.
      state = 1;
    } else {
      digitalWrite(DirPin,LOW);   // Change to Low.
      state = 0;
    }
  }  
  
  // Rotate in a certain direction until the timer expires
  if (TMRArd_IsTimerExpired(0)) {
    TMRArd_InitTimer(0,ticks*whole_time); // in seconds
    digitalWrite(DirPin,HIGH);   // Change to High.
  } else {
    digitalWrite(DirPin,LOW);   // Change to Low. 
  }
  
  // Obtain potentiometer reading
  unsigned int val = 0;
  val = analogRead(A0)/10;
  Serial.println(val);
  
  // Send pulse to stepper motor in a predefined period
  if (IsPulseFinished()) {
  Serial.println("Pulse Finished.");
  InitPulse(StepPin, period); 
  Pulse(val); 
  }
}
