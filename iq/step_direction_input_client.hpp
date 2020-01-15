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
  Name: step_direction_input_client.hpp
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef STEP_DIRECTION_INPUT_CLIENT_HPP_
#define STEP_DIRECTION_INPUT_CLIENT_HPP_


#include "client_communication.hpp"

const uint8_t kTypeStepDirInput = 58;

class StepDirectionInputClient: public ClientAbstract{
  public:
    StepDirectionInputClient(uint8_t obj_idn):
      ClientAbstract( kTypeStepDirInput, obj_idn),
      angle_(         kTypeStepDirInput, obj_idn, kSubAngle),
      angle_step_(    kTypeStepDirInput, obj_idn, kSubAngleStep)
      {};

    // Client Entries
    ClientEntry<float>      angle_;
    ClientEntry<float>      angle_step_;

    void ReadMsg(uint8_t* rx_data, uint8_t rx_length)
    {
      static const uint8_t kEntryLength = kSubAngleStep+1;
      ClientEntryAbstract* entry_array[kEntryLength] = {
        &angle_,      // 0
        &angle_step_  // 1
      };

      ParseMsg(rx_data, rx_length, entry_array, kEntryLength);
    }

  private:
    static const uint8_t kSubAngle               = 0;
    static const uint8_t kSubAngleStep           = 1;
};

#endif /* STEP_DIRECTION_INPUT_CLIENT_HPP_ */
