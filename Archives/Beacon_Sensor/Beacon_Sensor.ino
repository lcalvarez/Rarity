/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
 
 #include <Timers.h>
 #include <avr/io.h>
 #include <avr/interrupt.h>
 
#define LED_PIN 13
#define PHOTOTRANS_PORT A0
#define SAMPLE_PERIOD 100
#define SAMPLE_TIMER 0
#define VAL_MAX 1023
#define VAL_MIN 0

#define PRINT_TIMER 1
#define PRINT_PERIOD 1000

#define USEC_PER_SEC ((double) 1000000)

// the setup function runs once when you press reset or power the board
volatile int val = VAL_MIN;
volatile int hold = VAL_MIN;
volatile int value = 0;
/*
ISR(INT0_vect) {
  // check the value again - since it takes some time to
  // activate the interrupt routine, we get a clear signal.
  value = digitalRead(INTERRUPT_PIN);
}
*/
void setup() {
  Serial.begin(9600);
  digitalWrite(LED_PIN, LOW);
  
  attachInterrupt(0,rising,RISING);
  attachInterrupt(1,falling,FALLING);
  
  // initialize digital pin 13 as an output.
  pinMode(LED_PIN,OUTPUT);
  if (val > VAL_MAX) val = VAL_MAX;
  else if (val < VAL_MIN) val = VAL_MIN;
  TMRArd_InitTimer(SAMPLE_TIMER,SAMPLE_PERIOD);
  TMRArd_InitTimer(PRINT_TIMER,PRINT_PERIOD);
}

unsigned long prev = 0;
unsigned long diff = 0;
void rising() {
  digitalWrite(LED_PIN, HIGH);
  unsigned long temp = micros();
  diff = temp - prev;
  prev = temp;
  //Serial.println("RISE");
/*  if (!TMRArd_IsTimerExpired(SAMPLE_TIMER)) {
    Serial.println("HI");
    TMRArd_InitTimer(RISING_TIMER,RISING_PERIOD);
  }
*/
}

void falling() {
  digitalWrite(LED_PIN, LOW);
  //Serial.println("FALL");
}

// the loop function runs over and over again forever
void loop() {
  if (!TMRArd_IsTimerExpired(SAMPLE_TIMER)) {
    volatile int temp = analogRead(PHOTOTRANS_PORT);
    if (temp > hold) {
      hold = temp;
    }
    //Serial.println((int)hold);
  }
  else {
    val = hold;
    hold = VAL_MIN;
    TMRArd_InitTimer(SAMPLE_TIMER,SAMPLE_PERIOD);
  }

  if (TMRArd_IsTimerExpired(PRINT_TIMER)) {
    //Serial.println((int)val);
    Serial.print("useconds: ");
    Serial.println(diff);
    Serial.print("Frequency: ");
    if (diff) Serial.println(USEC_PER_SEC/diff);
    else Serial.println("BEACON NOT SENSED");
    diff = 0;
    
    TMRArd_InitTimer(PRINT_TIMER,PRINT_PERIOD);
  }
/*  
  digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delayMicroseconds(val+1);              // wait for a second
  digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delayMicroseconds(VAL_MAX+1-val);              // wait for a second
*/
}
