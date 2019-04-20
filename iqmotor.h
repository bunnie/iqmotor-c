#include <time.h>

#include "iq-module-communication-cpp/inc/generic_interface.hpp"
#include "iq-module-communication-cpp/inc/multi_turn_angle_control_client.hpp"

// Simplified functions for talking to IQ devices over COM ports
// Best for Linux users. Might even work for iOS. Probably doesn't work for windows.

extern struct timespec ts_ref;

typedef struct iqMotor {
  char *com_path;
  int fd;
  GenericInterface *iq_com;
  MultiTurnAngleControlClient *mta_client;  
} iqMotor;

// Create IqMotor object and bind a COM port path to it
// Call exactly once per COM port/motor pair to create a unique motor controller object
//
// Arguments:
//   char *path - path to UART object, e.g. "/dev/ttyUSB0"
// Returns: iqMotor control object
//
// Side effects: sets terminal options on the COM port for motor control
struct iqMotor *iqCreateMotor( const char *path );

// Set a motor to "coast" mode, so it's not driving a fixed position
// Arguments: motor object
int iqSetCoast( struct iqMotor *motor );

// read the angle of a motor
// Arguments: motor_obj (already initialized with prior call to createIqMotor)
// Returns: current angle of motor, in radians
double iqReadAngle( struct iqMotor *motor );

// set the angle of a motor
// Arguments: motor object
// target_angle: double that sets the target angle of the motor in radians
// trave_time_ms: target travel time to get to that angle -- may not be achievable if too short!
void iqSetAngle( struct iqMotor *motor, double target_angle, unsigned long travel_time_ms );

// set delta angle based on current angle
// Arguments: motor object
// target_angle: double that specifies the desired angular offset from whatever the current angle is
// trave_time_ms: target travel time to get to that angle -- may not be achievable if too short!
void iqSetAngleDelta( struct iqMotor *motor, double target_angle_delta, unsigned long travel_time_ms );

#define IQ_BUFLEN 1024   // length of IQ message buffer

/////////// CONVENIENCE FUNCTIONS

// convenience functions to help with real-time stuff
void elapsed_reset();
unsigned long elapsed_ms();

// conveinence functions to help with terminal IO
int set_interface_attribs (int fd, int speed, int parity);
void setup_console();  // setup console for non-blocking operation
int kb_hit();  // returns true if keyboard is hit
