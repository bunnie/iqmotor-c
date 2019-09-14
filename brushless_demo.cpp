#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <ctype.h>
#include <termios.h>

#include "iqmotor.h"

#define MOTOR0_PORT "/dev/ttyUSB0"

char kb_adjust() {
  char c;
  
  if( read(0, &c, 1) > 0 )
    return c;
  else
    return 0;
}

struct termios orig_termios;

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(int argc, char **argv) {
  struct iqMotor *motor0;

  if( argc != 2 ) {
    printf( "Missing speed argument, -1.0 to 1.0\n" );
    return 1;
  }
  float speed = atof(argv[1]);
  if( speed < -1.0 || speed > 1.0 ) {
    printf( "speed is out of range: %0.2f\n", speed );
    return 1;
  }
  
  /// now setup motor controller terminal properties
  motor0 = iqCreateMotor( MOTOR0_PORT );
  if( motor0 == NULL )
    return 1;

  //  iqSetCoast( motor0 );  // motor0 is the input, so put it in "coast"

  iqBlSetCoast(motor0); // put into coast mode

  iqBlSetPwm(motor0, speed);
  
  printf( "Press q to break.\n" );
  /// setup console terminal prooperties
  // setup_console();
  enableRawMode();

  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    if( c == 'a') {
      speed += 0.025;
      if( speed > 1.0 ) {
	speed = 1.0;
      }
      printf("%0.3f\n", speed);
      iqBlSetPwm(motor0, speed);
    }
    if( c == 'o' ) {
      speed -= 0.025;
      if( speed < -1.0 ) {
	speed = -1.0;
      }
      printf("%0.3f\n", speed);
      iqBlSetPwm(motor0, speed);
    }
    double velocity = iqBlReadVelocity( motor0 );
    printf( "%f rpm\r", (velocity / (2 * 3.14159265)) * 60.0 );
  }
  disableRawMode();
  
  iqBlSetPwm(motor0, 0.0);
  iqBlSetBreak(motor0);

  return 0;
}
