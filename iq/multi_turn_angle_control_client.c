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
#include "multi_turn_angle_control_client.h"
#include "../delay.h"

// mta storage array allocation
mta_storage entry_array_mta[kEntryLength] = {
  { false, kSubCtrlMode                       , mta_uint8_t, {.c = (uint8_t) 0} },
  { false, kSubCtrlBrake                      , mta_void, {.c = 0} },
  { false, kSubCtrlCoast                      , mta_void, {.c = 0} },
  { false, kSubCtrlAngle                      , mta_float, {.f = (float) 0} },
  { false, kSubCtrlVelocity                   , mta_float, {.f = (float) 0} },
  { false, kSubAngleKp                        , mta_float, {.f = (float) 0} },
  { false, kSubAngleKi                        , mta_float, {.f = (float) 0} },
  { false, kSubAngleKd                        , mta_float, {.f = (float) 0} },
  { false, kSubTimeout                        , mta_float, {.f = (float) 0} },
  { false, kSubCtrlPwm                        , mta_float, {.f = (float) 0} },
  { false, kSubCtrlVolts                      , mta_float, {.f = (float) 0} },
  { false, kSubObsAngularDisplacement         , mta_float, {.f = (float) 0} },
  { false, kSubObsAngularVelocity             , mta_float, {.f = (float) 0} },
  { false, kSubMeterPerRad                    , mta_float, {.f = (float) 0} },
  { false, kSubCtrlLinearDisplacement         , mta_float, {.f = (float) 0} },
  { false, kSubCtrlLinearVelocity             , mta_float, {.f = (float) 0} },
  { false, kSubObsLinearDisplacement          , mta_float, {.f = (float) 0} },
  { false, kSubObsLinearVelocity              , mta_float, {.f = (float) 0} },
  { false, kSubAngularSpeedMax                , mta_float, {.f = (float) 0} },
  { false, kSubTrajectoryAngularDisplacement  , mta_float, {.f = (float) 0} },
  { false, kSubTrajectoryAngularVelocity      , mta_float, {.f = (float) 0} },
  { false, kSubTrajectoryAngularAcceleration  , mta_float, {.f = (float) 0} },
  { false, kSubTrajectoryDuration             , mta_float, {.f = (float) 0} },
  { false, kSubTrajectoryLinearDisplacement   , mta_float, {.f = (float) 0} },
  { false, kSubTrajectoryLinearVelocity       , mta_float, {.f = (float) 0} },
  { false, kSubTrajectoryLinearAcceleration   , mta_float, {.f = (float) 0} },
};

void mta_init(struct mta_object *mta, struct CommInterface_storage *com, uint8_t obj_idn) {
  mta->com = com;
  mta->obj_idn = obj_idn;
}

void mta_get(struct mta_object *mta, mta_command cmd) {
  uint8_t tx_msg[2];
  tx_msg[0] = cmd;
  tx_msg[1] = (mta->obj_idn<<2) | kGet; // high six | low two
  CommInterface_SendPacket(mta->com, kTypeAngleMotorControl, tx_msg, 2);
}

void mta_set(struct mta_object *mta, mta_command cmd) {

  if( entry_array_mta[cmd].type == mta_void ) {
    uint8_t tx_msg[2];
    tx_msg[0] = cmd;
    tx_msg[1] = (mta->obj_idn<<2) | kSet; // high six | low two
    CommInterface_SendPacket(mta->com, kTypeAngleMotorControl, tx_msg, 2);
  } else if( entry_array_mta[cmd].type == mta_uint8_t ) {
    uint8_t tx_msg[2 + sizeof(uint8_t)];
    tx_msg[0] = cmd;
    tx_msg[1] = (mta->obj_idn<<2) | kSet; // high six | low two
    memcpy(&tx_msg[2], &(mta->data.data.c), sizeof(uint8_t));
    CommInterface_SendPacket(mta->com, kTypeAngleMotorControl, tx_msg, 2 + sizeof(uint8_t));
  } else if( entry_array_mta[cmd].type == mta_float ) {
    uint8_t tx_msg[2 + sizeof(float)];
    tx_msg[0] = cmd;
    tx_msg[1] = (mta->obj_idn<<2) | kSet; // high six | low two
    memcpy(&tx_msg[2], &mta->data.data.f, sizeof(float));
    CommInterface_SendPacket(mta->com, kTypeAngleMotorControl, tx_msg, 2 + sizeof(float));
  } else {
    printf("unknown command in mta_set\n");
  }
}

void mta_save(struct mta_object *mta, mta_command cmd) {
  uint8_t tx_msg[2];
  tx_msg[0] = cmd;
  tx_msg[1] = (mta->obj_idn<<2) | kSave; // high six | low two
  CommInterface_SendPacket(mta->com, kTypeAngleMotorControl, tx_msg, 2);
}

void mta_Reply(struct mta_object *mta, uint8_t *data, uint8_t len, mta_command cmd) {
  if( entry_array_mta[cmd].type == mta_void ) {
    if(len == 0) {
      mta->data.is_fresh = true;
    }
  } else if( entry_array_mta[cmd].type == mta_uint8_t ) {
    if(len == sizeof(uint8_t)) {
      memcpy(&mta->data.data.c, data, sizeof(uint8_t));
      mta->data.is_fresh = true;
    }
  } else if( entry_array_mta[cmd].type == mta_float ) {
    if(len == sizeof(float)) {
      memcpy(&mta->data.data.f, data, sizeof(float));
      mta->data.is_fresh = true;
    }
  } else {
    // this is an error condition, should probably print that
  }
}

void mta_get_reply(struct mta_object *mta) {
  // return value was presumably already set by a Reply call
  mta->data.is_fresh = false;
}

bool mta_IsFresh(struct mta_object *mta) {
  return mta->data.is_fresh;
}

