#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <ctype.h>
#include <termios.h>
#include <sys/time.h>
#include <fcntl.h>

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

long long current_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL);
  long long milliseconds = te.tv_sec*1000LL + te.tv_usec / 1000;
  return milliseconds;
}

int inputAvailable() {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return (FD_ISSET(0, &fds));
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
  long long ms = current_timestamp();
  while (1) {
    if( inputAvailable() ) {
      if( read(STDIN_FILENO, &c, 1) == 1 && c != 'q' ) {
	if( c == 'a') {
	  speed += 0.01;
	  if( speed > 1.0 ) {
	    speed = 1.0;
	  }
	  printf("%0.3f\n", speed);
	  iqBlSetPwm(motor0, speed);
	}
	if( c == 'o' ) {
	  speed -= 0.01;
	  if( speed < 0.0 ) {
	    speed = 0.0;
	  }
	  printf("%0.3f\n", speed);
	  iqBlSetPwm(motor0, speed);
	}
      }
      if( c == 'q' )
	break;
    }

    if( current_timestamp() - ms > 250 ) {
      float velocity = iqBlReadVelocity( motor0 );
      printf( "         %.0f rpm\n", (velocity / (2 * 3.14159265)) * 60.0 );
      //    printf( "\n%f rpm\n", velocity );
      ms = current_timestamp();
    }

    sleep(0.1);
  }
  
  disableRawMode();
  
  iqBlSetPwm(motor0, 0.0);
  iqBlSetBreak(motor0);

  return 0;
}
