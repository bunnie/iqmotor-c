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
  Name: multi_turn_angle_control_client.hpp
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "communication_interface.h"
#include "power_monitor_client.h"
#include "../delay.h"

pmc_storage entry_array_pmc[kEntryLength_pmc] = {
  { false, kSubVolts       , pmc_float, {.f = (float) 0} },
  { false, kSubAmps        , pmc_float, {.f = (float) 0} },
  { false, kSubWatts       , pmc_float, {.f = (float) 0} },
  { false, kSubJoules      , pmc_float, {.f = (float) 0} },
  { false, kSubResetJoules , pmc_void, {.c = 0} },
  { false, kSubFilterFs    , pmc_uint32_t, {.l = (uint32_t) 0} },
  { false, kSubFilterFc    , pmc_uint32_t, {.s = (uint32_t) 0} },
  { false, kSubVoltsRaw    , pmc_uint16_t, {.s = (uint16_t) 0} },
  { false, kSubAmpsRaw     , pmc_uint16_t, {.s = (uint16_t) 0} },
  { false, kSubVoltsGain   , pmc_float, {.f = (float) 0} },
  { false, kSubAmpsGain    , pmc_float, {.f = (float) 0} },
  { false, kSubAmpsBias    , pmc_float, {.f = (float) 0} },
};


void pmc_init(struct pmc_object *pmc, struct CommInterface_storage *com, uint8_t obj_idn) {
  pmc->com = com;
  pmc->obj_idn = obj_idn;
}

void pmc_get(struct pmc_object *pmc, pmc_command cmd) {
  uint8_t tx_msg[2];
  tx_msg[0] = cmd;
  tx_msg[1] = (pmc->obj_idn<<2) | kGet; // high six | low two
  CommInterface_SendPacket(pmc->com, kTypePowerMonitor, tx_msg, 2);
}

void pmc_set(struct pmc_object *pmc, pmc_command cmd) {

  if( entry_array_pmc[cmd].type == pmc_void ) {
    uint8_t tx_msg[2];
    tx_msg[0] = cmd;
    tx_msg[1] = (pmc->obj_idn<<2) | kSet; // high six | low two
    CommInterface_SendPacket(pmc->com, kTypePowerMonitor, tx_msg, 2);
  } else if( entry_array_pmc[cmd].type == pmc_uint8_t ) {
    uint8_t tx_msg[2 + sizeof(uint8_t)];
    tx_msg[0] = cmd;
    tx_msg[1] = (pmc->obj_idn<<2) | kSet; // high six | low two
    memcpy(&tx_msg[2], &(pmc->data.data.c), sizeof(uint8_t));
    CommInterface_SendPacket(pmc->com, kTypePowerMonitor, tx_msg, 2 + sizeof(uint8_t));
  } else if( entry_array_pmc[cmd].type == pmc_float ) {
    uint8_t tx_msg[2 + sizeof(float)];
    tx_msg[0] = cmd;
    tx_msg[1] = (pmc->obj_idn<<2) | kSet; // high six | low two
    memcpy(&tx_msg[2], &pmc->data.data.f, sizeof(float));
    CommInterface_SendPacket(pmc->com, kTypePowerMonitor, tx_msg, 2 + sizeof(float));
  } else if( entry_array_pmc[cmd].type == pmc_uint16_t ) {
    uint8_t tx_msg[2 + sizeof(uint16_t)];
    tx_msg[0] = cmd;
    tx_msg[1] = (pmc->obj_idn<<2) | kSet; // high six | low two
    memcpy(&tx_msg[2], &pmc->data.data.s, sizeof(uint16_t));
    CommInterface_SendPacket(pmc->com, kTypePowerMonitor, tx_msg, 2 + sizeof(uint16_t));
  } else if( entry_array_pmc[cmd].type == pmc_uint32_t ) {
    uint8_t tx_msg[2 + sizeof(uint32_t)];
    tx_msg[0] = cmd;
    tx_msg[1] = (pmc->obj_idn<<2) | kSet; // high six | low two
    memcpy(&tx_msg[2], &pmc->data.data.l, sizeof(uint32_t));
    CommInterface_SendPacket(pmc->com, kTypePowerMonitor, tx_msg, 2 + sizeof(uint32_t));
  } else {
    printf("unknown command in pmc_set\n");
  }
}

void pmc_save(struct pmc_object *pmc, pmc_command cmd) {
  uint8_t tx_msg[2];
  tx_msg[0] = cmd;
  tx_msg[1] = (pmc->obj_idn<<2) | kSave; // high six | low two
  CommInterface_SendPacket(pmc->com, kTypePowerMonitor, tx_msg, 2);
}

void pmc_Reply(struct pmc_object *pmc, uint8_t *data, uint8_t len, pmc_command cmd) {
  if( entry_array_pmc[cmd].type == pmc_void ) {
    if(len == 0) {
      pmc->data.is_fresh = true;
    }
  } else if( entry_array_pmc[cmd].type == pmc_uint8_t ) {
    if(len == sizeof(uint8_t)) {
      memcpy(&pmc->data.data.c, data, sizeof(uint8_t));
      pmc->data.is_fresh = true;
    }
  } else if( entry_array_pmc[cmd].type == pmc_float ) {
    if(len == sizeof(float)) {
      memcpy(&pmc->data.data.f, data, sizeof(float));
      pmc->data.is_fresh = true;
    }
  } else if( entry_array_pmc[cmd].type == pmc_uint16_t ) {
    if(len == sizeof(uint16_t)) {
      memcpy(&pmc->data.data.s, data, sizeof(uint16_t));
      pmc->data.is_fresh = true;
    }
  } else if( entry_array_pmc[cmd].type == pmc_uint32_t ) {
    if(len == sizeof(uint32_t)) {
      memcpy(&pmc->data.data.l, data, sizeof(uint32_t));
      pmc->data.is_fresh = true;
    }
  } else {
    // this is an error condition, should probably print that
  }
}

void pmc_get_reply(struct pmc_object *pmc) {
  // return value was presumably already set by a Reply call
  pmc->data.is_fresh = false;
}

bool pmc_IsFresh(struct pmc_object *pmc) {
  return pmc->data.is_fresh;
}

