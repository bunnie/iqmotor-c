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
  Name: communication_interface.hpp
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef COMMUNICATION_INTERFACE_H
#define	COMMUNICATION_INTERFACE_H

#include <stdint.h>

enum Access {kGet=0, kSet=1, kSave=2, kReply=3};

#include "packet_finder.h"
#include "byte_queue.h"
#include "bipbuffer.h"
#include "multi_turn_angle_control_client.h"
#include "power_monitor_client.h"

#define GENERIC_PF_INDEX_DATA_SIZE 20   // size of index buffer in packet_finder

#ifndef GENERIC_TX_BUFFER_SIZE
  #define GENERIC_TX_BUFFER_SIZE 64
#endif

// Member Variables
typedef struct CommInterface_storage {
  struct PacketFinder pf;        // packet_finder instance
  struct ByteQueue index_queue;              // needed by pf for storing indices
  uint8_t pf_index_data[GENERIC_PF_INDEX_DATA_SIZE]; // data for index_queue used by pf
  struct BipBuffer tx_bipbuf;   // bipbuffer for transmissions 
  uint8_t tx_buffer[GENERIC_TX_BUFFER_SIZE];   // raw buffer for transmissions
} CommInterface_storage;


    /*******************************************************************************
     * Receive
     ******************************************************************************/

    /// Poll the hardware for new byte data.
    ///   Returns: 1 packet ready
    ///            0 normal operation
    ///           -1 failure
    ///
int8_t CommInterface_GetBytes(struct CommInterface_storage *self);

    /// Peek at the next available incoming packet. If a packet is ready, pointer 
    /// 'packet' will point to the first byte of type+data and 'length' will give 
    /// the length of packet type+data. Arguments 'packet' and 'length' are ignored 
    /// if no packet is ready.  Repeated calls to Peek will return pointers to the 
    /// same packet data until Drop is used.
    ///   Returns: 1 packet peek available
    ///            0 no packet peek available
    ///           -1 failure
    ///
int8_t CommInterface_PeekPacket(struct CommInterface_storage *self, uint8_t **packet, uint8_t *length);

    /// Drop the next available packet from queue. Usually called after Peek.
    ///   Returns: 1 packet removed
    ///            0 no packet ready to remove
    ///           -1 failure
    ///
int8_t CommInterface_DropPacket(struct CommInterface_storage *self);


    /*******************************************************************************
     * Send
     ******************************************************************************/

    /// Add a packet to the outgoing USB queue with automatically generated header 
    /// and CRC. A hardware transmission is not immediately initiated unless the 
    /// endpoint is filled. To force a transmission, follow with SendNow(). This 
    /// operation is nonblocking. If the buffer fills, the most recent data is lost.
int8_t CommInterface_SendPacket(struct CommInterface_storage *self, uint8_t msg_type, uint8_t *data, uint16_t length);

    /// Add bytes to the outgoing USB queue. A hardware transmission is not 
    /// immediately initiated unless the endpoint is filled. To force a 
    /// transmission, follow with SendUsbNow(). This operation is 
    /// nonblocking. If the buffer fills, the most recent data is lost.
int8_t CommInterface_SendBytes(struct CommInterface_storage *self, uint8_t *bytes, uint16_t length);

    /// Initiate a hardware transmission, which will chain transmissions through 
    /// the endpoint IN interrupt until the buffer empties completely.
void CommInterface_SendNow(struct CommInterface_storage *self);
    
    /*******************************************************************************
     * Parsing
     ******************************************************************************/
     


    /// Gets all outbound bytes 
    /// The data is copied into the user supplied data_out buffer.
    /// The length of data transferred is copied into length_out.
    /// Returns: 1 for data transferred
    ///          0 for no data transferred (buffer empty)
int8_t CommInterface_SetRxBytes(struct CommInterface_storage *self, uint8_t* data_in, uint16_t length_in);
int8_t CommInterface_GetTxBytes(struct CommInterface_storage *self, uint8_t* data_out, uint8_t *length_out);
void CommInterface_init(struct CommInterface_storage *iq_com);

void CommInterface_ReadMsg_Mta(struct mta_object *mta, uint8_t* data, uint8_t length);
int8_t ParseMsg_Mta(struct mta_object *mta, uint8_t* rx_data, uint8_t rx_length);

void CommInterface_ReadMsg_Pmc(struct pmc_object *mta, uint8_t* data, uint8_t length);
int8_t ParseMsg_Pmc(struct pmc_object *mta, uint8_t* rx_data, uint8_t rx_length);


#endif // COMMUNICATION_INTERFACE_H
