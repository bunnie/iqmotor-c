/*
  Copyright 2019 IQinetics Technologies, Inc support@iq-control.com

  This file is part of the IQ C++ API.

  IQ C++ API is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  IQ C++ API is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/*
  Name: client_communication.cpp
  Last update: 4/12/2019 by Matthew Piccoli
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#include <stdio.h>
#include <stdint.h>
#include "crc_helper.h"
#include "string.h" // for memcpy
#include "bipbuffer.h"

#include "multi_turn_angle_control_client.h"
#include "power_monitor_client.h"
#include "communication_interface.h"

///////// EACH CLIENT GETS THIS SECTION TO HANDLE READS
extern mta_storage entry_array_mta[];
// lookup entry_length based on the mta->type field
int8_t ParseMsg_Mta(struct mta_object *mta, uint8_t* rx_data, uint8_t rx_length)
{
  uint8_t type_idn = rx_data[0];
  uint8_t sub_idn = rx_data[1];
  uint8_t obj_idn = rx_data[2] >> 2; // high 6 bits are obj_idn
  enum Access dir = (enum Access) (rx_data[2] & 0b00000011); // low two bits

  // if we have a reply (we only parse replies here)
  if(dir == kReply)
  {
    // if sub_idn is within array range (safe to access array at this location)
    if(sub_idn < kEntryLength)
      {
	//	printf( "kreply: sub %d, type %d, obj %d\n", sub_idn, type_idn, obj_idn );
	    // if the type and obj identifiers match
	if(//entry_array_mta[sub_idn].command == type_idn &&
	   kTypeAngleMotorControl == type_idn &&
		 mta->obj_idn == obj_idn)
		{
		  // ... then we have a valid message
		  mta_Reply(mta, &rx_data[3], rx_length-3, sub_idn);
		  return 1; // I parsed something
		}
	    }
    }
  return 0; // I didn't parse anything
}
void CommInterface_ReadMsg_Mta(struct mta_object *mta, uint8_t* data, uint8_t length)
{
  ParseMsg_Mta(mta, data, length);
}


extern pmc_storage entry_array_pmc[];
int8_t ParseMsg_Pmc(struct pmc_object *pmc, uint8_t* rx_data, uint8_t rx_length)
{
  uint8_t type_idn = rx_data[0];
  uint8_t sub_idn = rx_data[1];
  uint8_t obj_idn = rx_data[2] >> 2; // high 6 bits are obj_idn
  enum Access dir = (enum Access) (rx_data[2] & 0b00000011); // low two bits

  // if we have a reply (we only parse replies here)
  //printf( "parsemsg_pmc\n" );
  if(dir == kReply)
  {
    //    printf( "kreply: sub %d, type %d, obj %d\n", sub_idn, type_idn, obj_idn );
    // if sub_idn is within array range (safe to access array at this location)
    if(sub_idn < kEntryLength_pmc)
      {
	//printf( "sub_idn in range\n" );
	    // if the type and obj identifiers match
	if( // entry_array_pmc[sub_idn].command == type_idn &&
	   kTypePowerMonitor == type_idn &&
		 pmc->obj_idn == obj_idn)
		{
		  // ... then we have a valid message
		  pmc_Reply(pmc, &rx_data[3], rx_length-3, sub_idn);
		  //printf( "pmc reply called\n" );
		  return 1; // I parsed something
		}
	    }
    }
  return 0; // I didn't parse anything
}
void CommInterface_ReadMsg_Pmc(struct pmc_object *pmc, uint8_t* data, uint8_t length)
{
  ParseMsg_Pmc(pmc, data, length);
}
///////// END PER CLIENT SECITON

void CommInterface_init(struct CommInterface_storage *self)
{
  //printf( "initbq\n" );
  InitBQ(&self->index_queue, self->pf_index_data, GENERIC_PF_INDEX_DATA_SIZE);
  //printf( "initpkt\n" );
  InitPacketFinder(&self->pf, &self->index_queue);
  //printf( "bipbufferinit\n" );
  BipBuffer_init(&self->tx_bipbuf, self->tx_buffer, GENERIC_TX_BUFFER_SIZE); 
}

int8_t CommInterface_GetBytes(struct CommInterface_storage *self)
{
  // I can't do anything on my own since I don't have hardware
  // Use SetRxBytes(uint8_t* data_in, uint16_t length_in)
  return 0;
}

int8_t CommInterface_SetRxBytes(struct CommInterface_storage *self, uint8_t* data_in, uint16_t length_in)
{
  if(data_in == NULL)
    return -1;
  
  if(length_in)
  {
    //copy it over
    PutBytes(&self->pf, data_in, length_in); 
    return 1;
  }
  else
    return 0;
}

int8_t CommInterface_PeekPacket(struct CommInterface_storage *self, uint8_t **packet, uint8_t *length)
{
  return(PeekPacket(&self->pf, packet, length));
}

int8_t CommInterface_DropPacket(struct CommInterface_storage *self)
{
  return(DropPacket(&self->pf));
}

int8_t CommInterface_SendPacket(struct CommInterface_storage *self, uint8_t msg_type, uint8_t *data, uint16_t length)
{
  // This function must not be interrupted by another call to SendBytes or 
  // SendPacket, or else the packet it builds will be spliced/corrupted.

  uint8_t header[3];
  header[0] = kStartByte;                   // const defined by packet_finder.c
  header[1] = length;
  header[2] = msg_type;
  //printf("SendPacket SendBytes\n");
  //delay_ms(10);
  CommInterface_SendBytes(self, header, 3);
  
  //printf("SendPacket SendBytes\n");
  //delay_ms(10);
  CommInterface_SendBytes(self, data, length);
  
  uint8_t footer[2];
  uint16_t crc;
  //printf("SendPacket makecrc\n");
  //delay_ms(10);
  crc = MakeCrc(&(header[1]), 2);
  //printf("SendPacket updatecrc\n");
  //delay_ms(10);
  crc = ArrayUpdateCrc(crc, data, length);
  footer[0] = crc & 0x00FF;
  footer[1] = crc >> 8;
  //printf("SendPacket SendBytes\n");
  //delay_ms(10);
  CommInterface_SendBytes(self, footer, 2);
  
  return(1);
}

int8_t CommInterface_SendBytes(struct CommInterface_storage *self, uint8_t *bytes, uint16_t length)
{
  uint16_t length_temp = 0;
  uint8_t* location_temp;
  int8_t ret = 0;
    
  //printf("SendBytes Reserv\n");
  //delay_ms(10);
  // Reserve space in the buffer
  location_temp = self->tx_bipbuf.Reserve(&self->tx_bipbuf, length, &length_temp);
  
  // If there's room, do the copy
  if(length == length_temp)
  {
    //printf("SendBytes committing\n");
    //delay_ms(10);
    memcpy(location_temp, bytes, length_temp);   // do copy
    self->tx_bipbuf.Commit(&self->tx_bipbuf, length_temp);
    ret = 1;
  }
  else
  {
    //printf("SendBytes canceling\n");
    //delay_ms(10);
    self->tx_bipbuf.Commit(&self->tx_bipbuf, 0); // Call the restaurant, cancel the reservations
  }
    
  return ret;
}

int8_t CommInterface_GetTxBytes(struct CommInterface_storage *self, uint8_t* data_out, uint8_t *length_out)
{
  uint16_t length_temp;
  uint8_t* location_temp;
  
  location_temp = self->tx_bipbuf.GetContiguousBlock(&self->tx_bipbuf, &length_temp);
  if(length_temp)
  {
    memcpy(data_out, location_temp, length_temp);
    *length_out = length_temp;
    self->tx_bipbuf.DecommitBlock(&self->tx_bipbuf, length_temp);
    
    location_temp = self->tx_bipbuf.GetContiguousBlock(&self->tx_bipbuf, &length_temp);
    memcpy(&data_out[*length_out], location_temp, length_temp);
    *length_out = *length_out + length_temp;
    self->tx_bipbuf.DecommitBlock(&self->tx_bipbuf, length_temp);
    return 1;
  }
  return 0;
}

void CommInterface_SendNow(struct CommInterface_storage *self)
{
  // I'm useless.
}

