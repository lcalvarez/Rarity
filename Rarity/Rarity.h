#ifndef Rarity_h
#define Rarity_h

#include "Arduino.h"
#include "Servo.h"
#include "Timers.h"

// STATE
#define STATE_TIMER      1
#define STATE_DELAY      750 //ms
#define FIRST_FORWARD    9
#define BACKING_UP       10
#define TURNING          11
#define DRIVING_STRAIGHT 12
#define STOP             13
#define SLIGHT_FORWARD   14
#define GAME_OVER        15

// TANK DRIVE
#define EN_LEFT          6   // Connected to E1 (Enable Pin) on L293
#define DIR_LEFT         7   // Connected to D1 (Direction Pin) on L293
#define EN_RIGHT         5   // Connected to E2 (Enable Pin) on L293
#define DIR_RIGHT        4  // Connected to D2 (Direction Pin) on L293
#define TURN_DURATION    200
#define FORWARD_DURATION 1100
#define REF_SPEED        200

// BUMPERS
#define BACK_RIGHT       10
#define FRONT_RIGHT      11
#define FRONT_LEFT       9
#define BACK_LEFT        8

#define TWO_POINT        135
#define THREE_POINT      156 

class Rarity {
  public:
  
    Rarity(void);
    
    ~Rarity(void);
  
    void initialize(void);
  
    void setDriveSpeed(int left, int right); // HIGH is backward, LOW is forward determined by sign

    void setDriveMotor(char pin, int speed); // HIGH is backward, LOW is forward determined by sign

    boolean isBumperHit(char bumper); // LF LB RF RB

    boolean isReady(void); // needed to pause in between states

    void setShooterAngle(int angle); // for 2 pt or 3 pt (or 1 point)

    void setShooterPower(int power); // #define ON (HIGH) #define OFF (LOW)

    void updateBallRequest(void); // time in ms

    void transitionToState(char next, int duration = 0); // have #defines for state names
    
    char getState(void);
    
    void setTimer(int duration);
    
    boolean isTimerExpired(void);
    
  private:
    
    char state;
    Servo turret;
    Servo request;
    boolean requesting;
    
};

#endif

