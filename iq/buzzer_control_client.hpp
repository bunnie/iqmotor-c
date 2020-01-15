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
  Name: buzzer_control_client.hpp
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef BUZZER_CONTROL_CLIENT_H
#define BUZZER_CONTROL_CLIENT_H

#include "client_communication.hpp"

const uint8_t kTypeBuzzerControl = 61;

class BuzzerControlClient: public ClientAbstract{
  public:
    BuzzerControlClient(uint8_t obj_idn):
      ClientAbstract( kTypeBuzzerControl, obj_idn),
      ctrl_mode_(     kTypeBuzzerControl, obj_idn, kSubCtrlMode),
      ctrl_brake_(    kTypeBuzzerControl, obj_idn, kSubCtrlBrake),
      ctrl_coast_(    kTypeBuzzerControl, obj_idn, kSubCtrlCoast),
      ctrl_note_(     kTypeBuzzerControl, obj_idn, kSubCtrlNote),
      volume_max_(    kTypeBuzzerControl, obj_idn, kSubVolumeMax),
      hz_(            kTypeBuzzerControl, obj_idn, kSubHz),
      volume_(        kTypeBuzzerControl, obj_idn, kSubVolume),
      duration_(      kTypeBuzzerControl, obj_idn, kSubDuration)
      {};

    // Client Entries
    ClientEntryVoid       ctrl_mode_;
    ClientEntryVoid       ctrl_brake_;
    ClientEntryVoid       ctrl_coast_;
    ClientEntryVoid       ctrl_note_;
    ClientEntry<float>    volume_max_;
    ClientEntry<uint16_t> hz_;
    ClientEntry<uint8_t>  volume_;
    ClientEntry<uint16_t> duration_;

    void ReadMsg(uint8_t* rx_data, uint8_t rx_length)
    {
      static const uint8_t kEntryLength = kSubDuration+1;
      ClientEntryAbstract* entry_array[kEntryLength] = {
        &ctrl_mode_,  // 0
        &ctrl_brake_, // 1
        &ctrl_coast_, // 2
        &ctrl_note_,  // 3
        &volume_max_, // 4
        &hz_,         // 5
        &volume_,     // 6
        &duration_,   // 7
      };

      ParseMsg(rx_data, rx_length, entry_array, kEntryLength);
    }

  private:
    static const uint8_t kSubCtrlMode   = 0;
    static const uint8_t kSubCtrlBrake  = 1;
    static const uint8_t kSubCtrlCoast  = 2;
    static const uint8_t kSubCtrlNote   = 3;
    static const uint8_t kSubVolumeMax  = 4;
    static const uint8_t kSubHz         = 5;
    static const uint8_t kSubVolume     = 6;
    static const uint8_t kSubDuration   = 7;
};

#endif // BUZZER_CONTROL_CLIENT_H
