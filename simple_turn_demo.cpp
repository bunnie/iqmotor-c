#include <stdio.h>
#include <unistd.h>

#include "iqmotor.h"

void copyState();

#define MOTOR0_PORT "/dev/ttyUSB0"
#define MOTOR1_PORT "/dev/ttyUSB1"

#define UPDATE_RATE 50  // in ms

int main() {
  struct iqMotor *motor0;
  struct iqMotor *motor1;
  
  /// setup console terminal prooperties
  setup_console();

  /// now setup motor controller terminal properties
  motor0 = iqCreateMotor( MOTOR0_PORT );
  if( motor0 == NULL )
    return 1;

  motor1 = iqCreateMotor( MOTOR1_PORT );
  if( motor1 == NULL )
    return 1;
  
  iqSetCoast( motor0 );  // motor0 is the input, so put it in "coast"
  
  printf( "Press enter to break.\n" );
  while (1) {
    double angle;
    
    // kbhit code
    if( kb_hit() )
      break;

    iqSetAngle(motor1, 0, 2000);
    sleep(2);
    iqSetAngle(motor1, 6.28, 2000);
    sleep(2);
    iqSetAngle(motor1, 0, 2000);
    sleep(2);
    iqSetAngle(motor1, 6.28 * 2, 2000);
    sleep(2);
    iqSetAngle(motor1, 0, 2000);
    sleep(2);
    iqSetAngle(motor1, 6.28 * 3, 2000);
    sleep(2);
    iqSetAngle(motor1, 0, 2000);
    sleep(2);
    iqSetAngle(motor1, 6.28 * 10, 2000);
    sleep(2);
    iqSetAngle(motor1, 0, 2000);
    sleep(2);
  }

  return 0;
}
