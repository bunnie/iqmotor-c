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
  Name: servo_input_parser_client.hpp
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef SERVO_INPUT_PARSER_CLIENT_HPP_
#define SERVO_INPUT_PARSER_CLIENT_HPP_

#include "client_communication.hpp"

const uint8_t kTypeServoInputParser  =   78;

class ServoInputParserClient: public ClientAbstract{
  public:
    ServoInputParserClient(uint8_t obj_idn):
      ClientAbstract( kTypeServoInputParser, obj_idn),
      mode_(          kTypeServoInputParser, obj_idn, kSubMode),
      unit_min_(      kTypeServoInputParser, obj_idn, kSubUnitMin),
      unit_max_(      kTypeServoInputParser, obj_idn, kSubUnitMax)
      {};

    // Client Entries
    // Control commands
    ClientEntry<uint8_t>  mode_;
    ClientEntry<float>    unit_min_;
    ClientEntry<float>    unit_max_;

    void ReadMsg(uint8_t* rx_data, uint8_t rx_length)
    {
      static const uint8_t kEntryLength = kSubUnitMax+1;
      ClientEntryAbstract* entry_array[kEntryLength] = {
        &mode_,     // 0
        &unit_min_, // 1
        &unit_max_  // 2
      };

      ParseMsg(rx_data, rx_length, entry_array, kEntryLength);
    }

  private:
    static const uint8_t kSubMode =     0;
    static const uint8_t kSubUnitMin =  1;
    static const uint8_t kSubUnitMax =  2;
};

#endif /* SERVO_INPUT_PARSER_CLIENT_HPP_ */
