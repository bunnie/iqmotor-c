#include <stdio.h>
#include <unistd.h>

#include "iqmotor.h"

void copyState();

#define MOTOR0_PORT "/dev/ttyUSB0"

#define UPDATE_RATE 50  // in ms

int main() {
  struct iqMotor *motor0;
  
  /// setup console terminal prooperties
  setup_console();

  /// now setup motor controller terminal properties
  motor0 = iqCreateMotor( MOTOR0_PORT );
  if( motor0 == NULL )
    return 1;

  //  iqSetCoast( motor0 );  // motor0 is the input, so put it in "coast"
  
  printf( "Press enter to break.\n" );
  while (1) {
    double angle;
    
    // kbhit code
    if( kb_hit() )
      break;

    iqSetAngle(motor0, 3.14 * 2, 2000);  // radians: ~6.28 = 360 degrees
    sleep(2);
    iqSetAngle(motor0, 0, 2000);
    sleep(2);
  }

  return 0;
}
