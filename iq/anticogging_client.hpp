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
  Name: anticogging_client.hpp
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef ANTICOGGING_CLIENT_H
#define ANTICOGGING_CLIENT_H

#include "client_communication.hpp"

const uint8_t kTypeAnticogging = 71;

class AnticoggingClient: public ClientAbstract{
  public:
    AnticoggingClient(uint8_t obj_idn):
      ClientAbstract( kTypeAnticogging, obj_idn),
      table_size_(    kTypeAnticogging, obj_idn, kSubTableSize),
      is_data_valid_( kTypeAnticogging, obj_idn, kSubIsDataValid),
      is_enabled_(    kTypeAnticogging, obj_idn, kSubIsEnabled),
      erase_(         kTypeAnticogging, obj_idn, kSubErase),
      left_shift_(    kTypeAnticogging, obj_idn, kSubLeftShift)
      {};

    // Client Entries
    ClientEntry<uint16_t> table_size_;
    ClientEntry<uint8_t> is_data_valid_;
    ClientEntry<uint8_t> is_enabled_;
    ClientEntryVoid erase_;
    ClientEntry<uint8_t> left_shift_;

    void ReadMsg(uint8_t* rx_data, uint8_t rx_length)
    {
      static const uint8_t kEntryLength = kSubLeftShift+1;
      ClientEntryAbstract* entry_array[kEntryLength] = {
        &table_size_,     // 0
        &is_data_valid_,  // 1
        &is_enabled_,     // 2
        &erase_,          // 3
        &left_shift_      // 4
      };

      ParseMsg(rx_data, rx_length, entry_array, kEntryLength);
    }

  private:
    static const uint8_t kSubTableSize    = 0;
    static const uint8_t kSubIsDataValid  = 1;
    static const uint8_t kSubIsEnabled    = 2;
    static const uint8_t kSubErase        = 3;
    static const uint8_t kSubLeftShift    = 4;
};

#endif // ANTICOGGING_CLIENT_H
