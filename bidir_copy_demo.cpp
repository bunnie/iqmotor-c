#include <stdio.h>
#include <unistd.h>

#include "iqmotor.h"

void copyState();

#define MOTOR0_PORT "/dev/ttyUSB0"
#define MOTOR1_PORT "/dev/ttyUSB1"

#define UPDATE_RATE 40  // in ms

int main() {
  struct iqMotor *motor0;
  
  /// setup console terminal prooperties
  setup_console();

  /// now setup motor controller terminal properties
  motor0 = createIqMotor( MOTOR0_PORT );
  if( motor0 == NULL )
    return 1;
  
  printf( "Press enter to break.\n" );
  while (1) {
    // kbhit code
    if( kb_hit() )
      break;

    printf( "elapsed time in ms: %ld, angle1: %lf\n", elapsed_ms(), readAngle(motor0) );
    usleep((useconds_t) UPDATE_RATE * 1000);
  }

  return 0;
}
