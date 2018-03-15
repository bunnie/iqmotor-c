#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <math.h>

#include <time.h>

#include <linux/limits.h>

#include "iqmotor.h"

struct timespec ts_ref = {0, 0};

struct iqMotor *createIqMotor( const char *path ) {
  struct iqMotor *iqm = NULL;
  int pathlen;

  iqm = (struct iqMotor *) malloc(sizeof(struct iqMotor));
  if( iqm == NULL )
    return iqm;

  iqm->iq_com = new GenericInterface();
  iqm->mta_client = new MultiTurnAngleControlClient(0);

  pathlen = strlen(path);
  if( pathlen >= PATH_MAX ) {
    return(NULL);
  }
  iqm->com_path = (char *) malloc( pathlen );
  if( iqm->com_path == NULL ) {
    free( iqm );
    return NULL;
  }
  
  strncpy( iqm->com_path, path, pathlen );

  iqm->fd = open( iqm->com_path, O_RDWR | O_NOCTTY | O_SYNC);
  if( iqm->fd < 0 ) {
    printf("Error %d opening input controller COM port %s: %s", errno, iqm->com_path, strerror(errno));
    free(iqm->com_path);
    free(iqm);
    return NULL;
  }
  set_interface_attribs( iqm->fd, B115200, 0 );  // set speed to 115,200 bps, 8n1 (no parity)

  return iqm;
}

double readAngle( struct iqMotor *motor ) {
  // This buffer is for passing around messages.
  uint8_t communication_buffer_in[IQ_BUFLEN];
  uint8_t communication_buffer_out[IQ_BUFLEN];
  // Stores length of message to send or receive
  uint8_t communication_length_in;
  uint8_t communication_length_out;

  float angle;

  ///////////// READ THE INPUT CONTROLLER
  // Generate the set messages
  
  motor->mta_client->ctrl_coast_.set(*(motor->iq_com)); // put the input controller in "coast" mode
  motor->mta_client->obs_angular_displacement_.get(*(motor->iq_com)); // get the angular displacement

  // Grab outbound messages in the com queue, store into buffer
  // If it transferred something to communication_buffer...
  if(motor->iq_com->GetTxBytes(communication_buffer_in, communication_length_in)) {
    write(motor->fd, communication_buffer_in, communication_length_in);
  } else {
    return NAN;
  }
  
  usleep((useconds_t) 1 * 1000); // delay 1ms for serial data to transmit data...
  
  // Reads however many bytes are currently available
  communication_length_in = read(motor->fd, communication_buffer_in, IQ_BUFLEN);
  
  // Puts the recently read bytes into com's receive queue
  motor->iq_com->SetRxBytes(communication_buffer_in, communication_length_in);
  
  uint8_t *rx_data; // temporary pointer to received type+data bytes
  uint8_t rx_length; // number of received type+data bytes
  // while we have message packets to parse
  while(motor->iq_com->PeekPacket(&rx_data, &rx_length)) {
    // Share that packet with all client objects
    motor->mta_client->ReadMsg(*(motor->iq_com), rx_data, rx_length);
    
    // Once we're done with the message packet, drop it
    motor->iq_com->DropPacket();
  }

  //  if( motor->mta_client->obs_angular_displacement_.IsFresh() ) {
    angle = motor->mta_client->obs_angular_displacement_.get_reply();
  //  }
    
  return angle;
}

void setAngle( struct iqMotor *motor, double target_angle, unsigned long travel_time_ms ) {
  // This buffer is for passing around messages.
  uint8_t communication_buffer_in[IQ_BUFLEN];
  uint8_t communication_buffer_out[IQ_BUFLEN];
  // Stores length of message to send or receive
  uint8_t communication_length_in;
  uint8_t communication_length_out;

  /////////////// WRITE OUTPUT CONTROLLER
  // Generate the set messages
  motor->mta_client->trajectory_angular_displacement_.set(*(motor->iq_com), (float) target_angle);
  motor->mta_client->trajectory_duration_.set(*(motor->iq_com), (float) travel_time_ms / 1000.0 ); 

  motor->mta_client->obs_angular_displacement_.get(*(motor->iq_com));
  
  // Grab outbound messages in the com queue, store into buffer
  // If it transferred something to communication_buffer...
  if(motor->iq_com->GetTxBytes(communication_buffer_out, communication_length_out)) {
    write(motor->fd, communication_buffer_out, communication_length_out);
  }
}

void setAngleDelta( struct iqMotor *motor, double target_angle_delta, unsigned long travel_time_ms ) {
  double cur_angle;

  cur_angle = readAngle( motor );
  setAngle( motor, cur_angle + target_angle_delta, travel_time_ms );
}


void elapsed_init() {
  clock_gettime(CLOCK_MONOTONIC, &ts_ref);
}

unsigned long elapsed_ms() {
  struct timespec ts_cur;

  if( (ts_ref.tv_sec == 0) && (ts_ref.tv_nsec == 0) ) {  // we're 99.99999% sure it's an unitialized timespec
    elapsed_init();
  }
  
  clock_gettime(CLOCK_MONOTONIC, &ts_cur);
  return( (ts_cur.tv_sec - ts_ref.tv_sec) * 1000 + (ts_cur.tv_nsec - ts_ref.tv_nsec) / (1000 * 1000) );
}

void setup_console() {
  int term;
  
  term = fcntl(0, F_GETFL, 0);
  fcntl (0, F_SETFL, (term | O_NDELAY));
}

int kb_hit() {
  char c;
  
  if( read(0, &c, 1) > 0 )
    return 1;
  else
    return 0;
}

int set_interface_attribs (int fd, int speed, int parity) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) {
    printf ("error %d from tcgetattr", errno);
    return -1;
  }

  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break
  // as \000 chars
  tty.c_iflag &= ~IGNBRK;         // disable break processing
  tty.c_lflag = 0;                // no signaling chars, no echo,
  // no canonical processing
  tty.c_oflag = 0;                // no remapping, no delays
  tty.c_cc[VMIN]  = 0;            // read doesn't block
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr (fd, TCSANOW, &tty) != 0) {
    printf ("error %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}


