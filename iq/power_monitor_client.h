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
  Name: power_monitor_client.hpp
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef POWER_MONITOR_CLIENT_HPP_
#define POWER_MONITOR_CLIENT_HPP_

#define kTypePowerMonitor   69

typedef enum {
  pmc_void = 0,
  pmc_uint8_t = 1,
  pmc_float = 2,
  pmc_uint16_t = 3,
  pmc_uint32_t = 4,
} pmc_type;

typedef enum {
    kSubVolts =      0,
    kSubAmps =       1,
    kSubWatts =      2,
    kSubJoules =     3,
    kSubResetJoules =4,
    kSubFilterFs =   5,
    kSubFilterFc =   6,
    kSubVoltsRaw =   7,
    kSubAmpsRaw =    8,
    kSubVoltsGain =  9,
    kSubAmpsGain =   10,
    kSubAmpsBias =   11
} pmc_command;

typedef struct pmc_storage {
  bool is_fresh;
  pmc_command command;
  pmc_type type;
  union {
    uint8_t c;
    float f;
    uint16_t s;
    uint32_t l;
  } data;
} pmc_storage;

typedef struct pmc_object {
  struct CommInterface_storage *com;
  uint8_t obj_idn;
  pmc_storage data; // item for holding the current data
} pmc_object;

void pmc_init(struct pmc_object *mta, struct CommInterface_storage *com, uint8_t obj_idn);
void pmc_get(struct pmc_object *mta, pmc_command cmd);
void pmc_set(struct pmc_object *mta, pmc_command cmd); // data to set is in mta->
void pmc_save(struct pmc_object *mta, pmc_command cmd);
void pmc_Reply(struct pmc_object *mta, uint8_t *data, uint8_t len, pmc_command cmd);
void pmc_get_reply(struct pmc_object *mta); // reply is in mta->data.type, mta->data.data; all other fields ignored
bool pmc_IsFresh(struct pmc_object *mta);

#define kEntryLength_pmc  (kSubAmpsBias+1)

#endif /* POWER_MONITOR_CLIENT_HPP_ */
