#include <time.h>

#include "communication_interface.h"
#include "multi_turn_angle_control_client.h"

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

// Simplified functions for talking to IQ devices over COM ports
// Best for Linux users. Might even work for iOS. Probably doesn't work for windows.

extern struct timespec ts_ref;

typedef struct iqMotor {
  char *com_path;
  struct CommInterface_storage *iq_com;
  struct mta_object *mta;
  struct CommInterface_storage *iq_pmc_com;
  struct pmc_object *pmc;
} iqMotor;

// Create IqMotor object and bind a COM port path to it
// Call exactly once per COM port/motor pair to create a unique motor controller object
//
// Arguments:
//   char *path - path to UART object, e.g. "/dev/ttyUSB0"
// Returns: iqMotor control object
//
// Side effects: sets terminal options on the COM port for motor control
 void iqCreateMotor( void );

// Set a motor to "coast" mode, so it's not driving a fixed position
// Arguments: motor object
 int iqSetCoast( void );

// read the angle of a motor
// Arguments: motor_obj (already initialized with prior call to createIqMotor)
// Returns: current angle of motor, in radians
 float iqReadAngle( void );

// set the angle of a motor
// Arguments: motor object
// target_angle: float that sets the target angle of the motor in radians
// trave_time_ms: target travel time to get to that angle -- may not be achievable if too short!
 void iqSetAngle( float target_angle, unsigned long travel_time_ms );

// set delta angle based on current angle
// Arguments: motor object
// target_angle: float that specifies the desired angular offset from whatever the current angle is
// trave_time_ms: target travel time to get to that angle -- may not be achievable if too short!
 void iqSetAngleDelta( float target_angle_delta, unsigned long travel_time_ms );


float iqReadAmps( void );

#define IQ_BUFLEN 1024   // length of IQ message buffer

