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
  Name: persistent_memory_client.hpp
  Last update: 4/12/2019 by Matthew Piccoli
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef PERSISTENT_MEMORY_CLIENT_HPP_
#define PERSISTENT_MEMORY_CLIENT_HPP_

#include "client_communication.hpp"

const uint8_t kTypePersistentMemory  =   11;

class PowerMonitorClient: public ClientAbstract{
  public:
    PowerMonitorClient(uint8_t obj_idn):
      ClientAbstract(     kTypePersistentMemory, obj_idn),
      erase_(             kTypePersistentMemory, obj_idn, kSubErase),
      revert_to_default_( kTypePersistentMemory, obj_idn, kSubRevertToDefault)
      {};

    // Client Entries
    // Control commands
    ClientEntryVoid   erase_;
    ClientEntryVoid   revert_to_default_;


    void ReadMsg(uint8_t* rx_data, uint8_t rx_length)
    {
      static const uint8_t kEntryLength = kSubRevertToDefault+1;
      ClientEntryAbstract* entry_array[kEntryLength] = {
        &erase_,            // 0
        &revert_to_default_ // 1
      };

      ParseMsg(rx_data, rx_length, entry_array, kEntryLength);
    }

  private:
    static const uint8_t kSubErase =            0;
    static const uint8_t kSubRevertToDefault =  1;
};

#endif /* PERSISTENT_MEMORY_CLIENT_HPP_ */
