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

    angle = iqReadAngle(motor0);
    printf( "elapsed time in ms: %ld, angle1: %lf\n", elapsed_ms(), angle );

    iqSetAngle(motor1, angle, UPDATE_RATE - 2);
	     
    usleep((useconds_t) UPDATE_RATE * 1000);
  }

  return 0;
}
