#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "iqmotor.h"

void copyState();

#define MOTOR0_PORT "/dev/ttyUSB0"
#define MOTOR1_PORT "/dev/ttyUSB1"

#define UPDATE_RATE 40  // in ms

#define ANGLE_TOL  (6.28 / 60.0) // tolerance off ideal to detect a slave trying to be master
#define SWITCH_TIMEOUT 250 // how long between master/slave swaps, in ms

#define IDLE_THRESH  0.002 // delta angle before idle timer runs
#define IDLE_TIMEOUT 300 // how long to be settled before we're "idle" in ms

int main() {
  struct iqMotor *motor0;
  struct iqMotor *motor1;

  struct iqMotor *master;
  struct iqMotor *slave;

  unsigned long sw_time = elapsed_ms();
  unsigned long idle_time = elapsed_ms();
  
  /// setup console terminal prooperties
  setup_console();

  /// now setup motor controller terminal properties
  motor0 = iqCreateMotor( MOTOR0_PORT );
  if( motor0 == NULL )
    return 1;

  motor1 = iqCreateMotor( MOTOR1_PORT );
  if( motor1 == NULL )
    return 1;
  

  printf( "Press enter to break.\n" );
  master = motor0;
  slave = motor1;
  iqSetCoast( master ); // the master "reads" and thus should be coasting, not driven
  double master_angle = 0.0;
  double slave_angle = 0.0;
  double last_angle = 0.0;
  while (1) {
    
    // kbhit code
    if( kb_hit() )
      break;

    if( fabs( last_angle - master_angle ) > IDLE_THRESH ) {
      idle_time = elapsed_ms();
    }
    last_angle = master_angle;
    master_angle = iqReadAngle(master);
    printf( "elapsed time in ms: %ld, angle1: %lf\n", elapsed_ms(), master_angle );

    iqSetAngle(slave, master_angle, UPDATE_RATE - 4);
	     
    usleep((useconds_t) UPDATE_RATE * 1000);
    
    slave_angle = iqReadAngle(slave);

    if( fabs(slave_angle - master_angle) > ANGLE_TOL ) {
      if( (elapsed_ms() - sw_time > SWITCH_TIMEOUT) && (elapsed_ms() - idle_time > IDLE_TIMEOUT)) {
	if( master == motor0 ) {
	  master = motor1;
	  slave = motor0;
	} else {
	  master = motor0;
	  slave = motor1;
	}
	iqSetCoast( master );
	sw_time = elapsed_ms();
      }
    }
  }

  return 0;
}
