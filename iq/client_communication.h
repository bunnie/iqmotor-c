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
  Name: client_communication.hpp
  Last update: 4/12/2019 by Matthew Piccoli
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef CLIENT_COMMUNICATION_H
#define CLIENT_COMMUNICATION_H

#include <string.h> // for memcpy
#include "communication_interface.h"

enum Access {kGet=0, kSet=1, kSave=2, kReply=3};

class ClientEntryVoid: public ClientEntryAbstract {
  public:
    ClientEntryVoid(uint8_t type_idn, uint8_t obj_idn, uint8_t sub_idn):
      ClientEntryAbstract(type_idn, obj_idn, sub_idn),
      is_fresh_(false)
      {};

    void get(CommunicationInterface &com) {
      uint8_t tx_msg[2];
      tx_msg[0] = sub_idn_;
      tx_msg[1] = (obj_idn_<<2) | kGet; // high six | low two
      com.SendPacket(type_idn_, tx_msg, 2);
    };

    void set(CommunicationInterface &com) {
      uint8_t tx_msg[2]; // must fit outgoing message
      tx_msg[0] = sub_idn_;
      tx_msg[1] = (obj_idn_<<2) | kSet; // high six | low two
      com.SendPacket(type_idn_, tx_msg, 2);
    }

    void save(CommunicationInterface &com) {
      uint8_t tx_msg[2];
      tx_msg[0] = sub_idn_;
      tx_msg[1] = (obj_idn_<<2) | kSave; // high six | low two
      com.SendPacket(type_idn_, tx_msg, 2);
    }

    void Reply(const uint8_t* data, uint8_t len) {
      (void)data;
      if(len == 0) {
        is_fresh_ = true;
      }
    };

    bool IsFresh() {return is_fresh_;};

  private:
    bool is_fresh_;
};

template <typename T>
class ClientEntry: public ClientEntryAbstract {
  public:
    ClientEntry(uint8_t type_idn, uint8_t obj_idn, uint8_t sub_idn):
      ClientEntryAbstract(type_idn, obj_idn, sub_idn),
      is_fresh_(false),
      value_()
      {};

    void get(CommunicationInterface &com) {
      uint8_t tx_msg[2];
      tx_msg[0] = sub_idn_;
      tx_msg[1] = (obj_idn_<<2) | kGet; // high six | low two
      com.SendPacket(type_idn_, tx_msg, 2);
    };

    void set(CommunicationInterface &com, T value) {
      uint8_t tx_msg[2+sizeof(T)]; // must fit outgoing message
      tx_msg[0] = sub_idn_;
      tx_msg[1] = (obj_idn_<<2) | kSet; // high six | low two
      memcpy(&tx_msg[2], &value, sizeof(T));
      com.SendPacket(type_idn_, tx_msg, 2+sizeof(T));
    }

    void save(CommunicationInterface &com) {
      uint8_t tx_msg[2];
      tx_msg[0] = sub_idn_;
      tx_msg[1] = (obj_idn_<<2) | kSave; // high six | low two
      com.SendPacket(type_idn_, tx_msg, 2);
    }

    void Reply(const uint8_t* data, uint8_t len) {
      if(len == sizeof(T)) {
        memcpy(&value_, data, sizeof(T));
        is_fresh_ = true;
      }
    };

    T get_reply() {
      is_fresh_ = false;
      return value_;
    };

    bool IsFresh() {return is_fresh_;};

  private:
    bool is_fresh_;
    T value_;
};

int8_t ParseMsg(uint8_t* rx_data, uint8_t rx_length,
  ClientEntryAbstract** entry_array, uint8_t entry_length);

int8_t ParseMsg(uint8_t* rx_data, uint8_t rx_length,
  ClientEntryAbstract& entry);

/*
  MultiTurnAngleClient consists of:
   - a series of commands, indexed 0-25 in an array with symbolic names
   - each command has a "value" associated with it that has a length, which could be 0, and a datatype, which is void, uint8_t, or float.
   - each command has a "is_fresh_" parameter associated with it
   - each command should respond to init, get, set, save, Reply, get_reply, isFresh methods
   - the trick in going from C++ -> C is the dynamic type dispatch

   type_idn is fixed: kTypeAngleMotorControl (changes based only on API type)
   obj_idn is tracked by the user code, as a passed parameter. obj_idn should be a pointer to a struct with all the data we need on otherwise
     generic functions
   sub_idn codes -- we can make an Enum list that encapsulates all the sub_idn_ codes.
     * This enum list should go into a lookup table that defines the dispatch type for the function
     * A function pointer should be assigned to the array entry according to the dispatch type.
 */

// we need float, void, and uint8_t variants
typedef struct Client_storage {
  uint8_t type_idn_;
  uint8_t obj_idn_;
  uint8_t sub_idn_;
  union {
    uint8_t value_uint8_t;
    float value_float_t;
  }
  
} Client_storage;

#endif // CLIENT_COMMUNICATION_H
