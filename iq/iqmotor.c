#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <time.h>
#include <generated/csr.h>
#include <generated/mem.h>
#include <hw/flags.h>
#include <console.h>
#include <system.h>

#include <errno.h>
#include <string.h>
#include <math.h>

#include <time.h>

//#include <linux/limits.h>

#include "iqmotor.h"
#include "../motor.h"
#include "../delay.h"

static struct iqMotor motor_storage;
static struct iqMotor *motor;
static struct CommInterface_storage iq_com;
static struct mta_object iq_mta;
static struct pmc_object iq_pmc;
static struct CommInterface_storage iq_pmc_com;

size_t write(const void *buf, size_t count);
size_t read(const void *buf, size_t count);

void iqCreateMotor(void) {
  // basic storage allocation
  motor = &motor_storage;


  // mta init
  motor->iq_com = &iq_com;
  CommInterface_init(motor->iq_com);
  
  motor->mta = &iq_mta;
  mta_init(motor->mta, motor->iq_com, 0);


  // pmc init
  motor->iq_pmc_com = &iq_pmc_com;
  CommInterface_init(motor->iq_pmc_com);
  
  motor->pmc = &iq_pmc;
  pmc_init(motor->pmc, motor->iq_pmc_com, 0);
}


 int iqSetCoast( void ) {
  // This buffer is for passing around messages.
  uint8_t communication_buffer_in[IQ_BUFLEN];
  // Stores length of message to send or receive
  uint8_t communication_length_in;

  //printf("calling mta_set\n");
  //delay_ms(10);
  mta_set(motor->mta, kSubCtrlCoast);  //  motor->mta_client->ctrl_coast_.set(*(motor->iq_com)); // put the input controller in "coast" mode
  // Grab outbound messages in the com queue, store into buffer
  // If it transferred something to communication_buffer...
  //printf("sending bytes\n");
  //delay_ms(10);
  if(CommInterface_GetTxBytes(motor->iq_com, communication_buffer_in, &communication_length_in)) {
    write(communication_buffer_in, communication_length_in);
    return 0;
  } else {
    return 1;
  }
}


 float iqReadAngle( void ) {
  // This buffer is for passing around messages.
  uint8_t communication_buffer_in[IQ_BUFLEN];
  uint8_t communication_buffer_out[IQ_BUFLEN];
  // Stores length of message to send or receive
  uint8_t communication_length_out;
  uint16_t communication_length_rx;

  float angle;
  int got_value = 0;

  ///////////// READ THE INPUT CONTROLLER
  // Generate the set messages
  mta_get(motor->mta, kSubObsAngularDisplacement); //  motor->mta_client->obs_angular_displacement_.get(*(motor->iq_com)); // get the angular displacement

  // Grab outbound messages in the com queue, store into buffer
  // If it transferred something to communication_buffer...
  if(CommInterface_GetTxBytes(motor->iq_com, communication_buffer_out, &communication_length_out)) {
    write(communication_buffer_out, communication_length_out);
  } else {
    return -1; // should be NAN...
  }
  
  delay(2); // delay 2ms for serial data to transmit data...
  
  // Reads however many bytes are currently available
  communication_length_rx = read(communication_buffer_in, IQ_BUFLEN);
  
  // Puts the recently read bytes into com's receive queue
  CommInterface_SetRxBytes(motor->iq_com, communication_buffer_in, communication_length_rx);
  
  uint8_t *rx_data; // temporary pointer to received type+data bytes
  uint8_t rx_length; // number of received type+data bytes
  // while we have message packets to parse
  while(CommInterface_PeekPacket(motor->iq_com, &rx_data, &rx_length)) {
    // Share that packet with all client objects
    //    motor->mta_client->ReadMsg(*(motor->iq_com), rx_data, rx_length);
    CommInterface_ReadMsg_Mta(motor->mta, rx_data, rx_length);
    
    // Once we're done with the message packet, drop it
    CommInterface_DropPacket(motor->iq_com);
    if( !got_value ) {
      angle = motor->mta->data.data.f;
      got_value = 1;
    }
  }

  //  if( motor->mta_client->obs_angular_displacement_.IsFresh() ) {
  mta_get_reply(motor->mta);
  //angle = motor->mta->data.data.f; // we could dispatch on type, but in this case, we "just know"
  //  }
    
  return angle;
}

 void iqSetAngle( float target_angle, unsigned long travel_time_ms ) {
  // This buffer is for passing around messages.
  uint8_t communication_buffer_out[IQ_BUFLEN];
  // Stores length of message to send or receive
  uint8_t communication_length_out;

  /////////////// WRITE OUTPUT CONTROLLER
  motor->mta->data.data.c = 6;
  mta_set(motor->mta, kSubCtrlMode);
  // motor->mta_client->ctrl_mode_.set(*(motor->iq_com), 6); // put the input controller in "coast" mode
  
  // Generate the set messages
  motor->mta->data.data.f = (float) target_angle;
  mta_set(motor->mta, kSubTrajectoryAngularDisplacement); //  motor->mta_client->trajectory_angular_displacement_.set(*(motor->iq_com), (float) target_angle);
  motor->mta->data.data.f = (float) travel_time_ms / 1000.0;
  mta_set(motor->mta, kSubTrajectoryDuration); //  motor->mta_client->trajectory_duration_.set(*(motor->iq_com), (float) travel_time_ms / 1000.0 ); 

  mta_get(motor->mta, kSubObsAngularDisplacement);
  // motor->mta_client->obs_angular_displacement_.get(*(motor->iq_com));
  
  // Grab outbound messages in the com queue, store into buffer
  // If it transferred something to communication_buffer...
  if(CommInterface_GetTxBytes(motor->iq_com, communication_buffer_out, &communication_length_out)) {
    write(communication_buffer_out, communication_length_out);
  }
}

void iqSetAngleDelta( float target_angle_delta, unsigned long travel_time_ms ) {
  float cur_angle;

  cur_angle = iqReadAngle();
  iqSetAngle(cur_angle + target_angle_delta, travel_time_ms );
}


///////// BEGIN PMC SECTION

float iqReadAmps( void ) {
  // This buffer is for passing around messages.
  uint8_t communication_buffer_in[IQ_BUFLEN];
  // Stores length of message to send
  uint8_t communication_length_in;
  // storel length of messages received
  uint16_t communication_length_rx;

  float amps;
  int got_value = 0;

  /*
  memset(&iq_pmc, 0, sizeof(iq_pmc));
  memset(&iq_pmc_com, 0, sizeof(iq_pmc));
  motor->iq_pmc_com = &iq_pmc_com;
  CommInterface_init(motor->iq_pmc_com);
  
  motor->pmc = &iq_pmc;
  pmc_init(motor->pmc, motor->iq_pmc_com, 0);
  */
  
  ///////////// READ THE INPUT CONTROLLER
  // Generate the set messages
  pmc_get(motor->pmc, kSubAmps); 

  // Grab outbound messages in the com queue, store into buffer
  // If it transferred something to communication_buffer...
  if(CommInterface_GetTxBytes(motor->iq_pmc_com, communication_buffer_in, &communication_length_in)) {
    write(communication_buffer_in, communication_length_in);
  } else {
    return -1; // should be NAN...
  }
  
  delay(2); // delay 2ms for serial data to transmit data...
  
  // Reads however many bytes are currently available
  communication_length_rx = read(communication_buffer_in, IQ_BUFLEN);
  //printf( "read length: %d\n", communication_length_rx );

  // Puts the recently read bytes into com's receive queue
  CommInterface_SetRxBytes(motor->iq_pmc_com, communication_buffer_in, communication_length_rx);
  
  uint8_t *rx_data; // temporary pointer to received type+data bytes
  uint8_t rx_length; // number of received type+data bytes
  // while we have message packets to parse
  while(CommInterface_PeekPacket(motor->iq_pmc_com, &rx_data, &rx_length)) {
    // Share that packet with all client objects
    CommInterface_ReadMsg_Pmc(motor->pmc, rx_data, rx_length);
    
    // Once we're done with the message packet, drop it
    CommInterface_DropPacket(motor->iq_pmc_com);
    if( !got_value ) {  // some weird bug causes multiple returns, and the later ones are bogus
      amps = motor->pmc->data.data.f;
      got_value = 1;
    }
    // printf( "dbg amps: %dmA\n", (int) (motor->pmc->data.data.f * 1000.0) );
  }

  pmc_get_reply(motor->pmc);
  //  amps = motor->pmc->data.data.f; // we could dispatch on type, but in this case, we "just know"
    
  return amps;
}

size_t write(const void *buf, size_t count) {
  int i = 0;
  char *wbuf = (char *) buf;
  //  printf("motor write: ");
  for( i = 0; i < count; i++ ) {
    //printf( "%02x ", wbuf[i] );
    motor_write(wbuf[i]);
    motor_sync(); // make sure we don't get ahead of ourselves sending data
  }
  //printf( "\n" );
  return count;
}

size_t read(const void *buf, size_t count) {
  int i = 0;
  int timeout = 0;
  char *rbuf = (char *)buf;

  //  printf("motor read: ");
  for( i = 0; i < count; i++ ) {
    if( motor_read_nonblock() )
      rbuf[i] = motor_read();
    else {
      timeout++;
      if( timeout > 100 ) {
	//printf( "read() timeout\n" );
	break;
      }
    }
    //    printf( "%02x ", rbuf[i] );
  }
  //  printf("\n");
  return (size_t) i;
}

