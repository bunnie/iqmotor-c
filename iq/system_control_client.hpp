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
  Name: system_control_client.hpp
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Matthew Piccoli
  Contributors: Raphael Van Hoffelen
*/

#ifndef SYSTEM_CONTROL_CLIENT_H
#define SYSTEM_CONTROL_CLIENT_H

#include "client_communication.hpp"

const uint8_t kTypeSystemControl = 5;

class SystemControlClient: public ClientAbstract{
  public:
    SystemControlClient(uint8_t obj_idn):
      ClientAbstract(       kTypeSystemControl, obj_idn),
      reboot_program_(      kTypeSystemControl, obj_idn,kSubRebootProgram),
      reboot_boot_loader_(  kTypeSystemControl, obj_idn,kSubRebootBootLoader),
      dev_id_(              kTypeSystemControl, obj_idn, kSubDevId),
      rev_id_(              kTypeSystemControl, obj_idn, kSubRevId),
      uid1_(                kTypeSystemControl, obj_idn, kSubUid1),
      uid2_(                kTypeSystemControl, obj_idn, kSubUid2),
      uid3_(                kTypeSystemControl, obj_idn, kSubUid3),
      mem_size_(            kTypeSystemControl, obj_idn, kSubMemSize),
      build_year_(          kTypeSystemControl, obj_idn, kSubBuildYear),
      build_month_(         kTypeSystemControl, obj_idn, kSubBuildMonth),
      build_day_(           kTypeSystemControl, obj_idn, kSubBuildDay),
      build_hour_(          kTypeSystemControl, obj_idn, kSubBuildHour),
      build_minute_(        kTypeSystemControl, obj_idn, kSubBuildMinuite),
      build_second_(        kTypeSystemControl, obj_idn, kSubBuildSecond),
      module_id_(           kTypeSystemControl, obj_idn, kSubModuleId),
      time_(                kTypeSystemControl, obj_idn, kSubTime),
      firmware_version_(    kTypeSystemControl, obj_idn, kSubFirmwareVersion),
      hardware_version_(    kTypeSystemControl, obj_idn, kSubHardwareVersion),
      electronics_version_( kTypeSystemControl, obj_idn, kSubElectronicsVersion),
      firmware_valid_(      kTypeSystemControl, obj_idn, kSubFirmwareValid)
      {};

    // Client Entries
    ClientEntryVoid       reboot_program_;
    ClientEntryVoid       reboot_boot_loader_;
    ClientEntry<uint16_t> dev_id_;
    ClientEntry<uint16_t> rev_id_;
    ClientEntry<uint32_t> uid1_;
    ClientEntry<uint32_t> uid2_;
    ClientEntry<uint32_t> uid3_;
    ClientEntry<uint16_t> mem_size_;
    ClientEntry<uint16_t> build_year_;
    ClientEntry<uint8_t>  build_month_;
    ClientEntry<uint8_t>  build_day_;
    ClientEntry<uint8_t>  build_hour_;
    ClientEntry<uint8_t>  build_minute_;
    ClientEntry<uint8_t>  build_second_;
    ClientEntry<uint8_t>  module_id_;
    ClientEntry<float>    time_;
    ClientEntry<uint32_t> firmware_version_;
    ClientEntry<uint32_t> hardware_version_;
    ClientEntry<uint32_t> electronics_version_;
    ClientEntry<uint8_t>  firmware_valid_;

    void ReadMsg(uint8_t* rx_data, uint8_t rx_length)
    {
      static const uint8_t kEntryLength = kSubFirmwareValid+1;
      ClientEntryAbstract* entry_array[kEntryLength] = {
        &reboot_program_,     // 0
        &reboot_boot_loader_, // 1
        &dev_id_,             // 2
        &rev_id_,             // 3
        &uid1_,               // 4
        &uid2_,               // 5
        &uid3_,               // 6
        &mem_size_,           // 7
        &build_year_,         // 8
        &build_month_,        // 9
        &build_day_,          // 10
        &build_hour_,         // 11
        &build_minute_,       // 12
        &build_second_,       // 13
        &module_id_,          // 14
        &time_,               // 15
        &firmware_version_,   // 16
        &hardware_version_,   // 17
        &electronics_version_,// 18
        &firmware_valid_      // 19
      };

      ParseMsg(rx_data, rx_length, entry_array, kEntryLength);
    }

  private:
    static const uint8_t kSubRebootProgram      = 0;
    static const uint8_t kSubRebootBootLoader   = 1;
    static const uint8_t kSubDevId              = 2;
    static const uint8_t kSubRevId              = 3;
    static const uint8_t kSubUid1               = 4;
    static const uint8_t kSubUid2               = 5;
    static const uint8_t kSubUid3               = 6;
    static const uint8_t kSubMemSize            = 7;
    static const uint8_t kSubBuildYear          = 8;
    static const uint8_t kSubBuildMonth         = 9;
    static const uint8_t kSubBuildDay           = 10;
    static const uint8_t kSubBuildHour          = 11;
    static const uint8_t kSubBuildMinuite       = 12;
    static const uint8_t kSubBuildSecond        = 13;
    static const uint8_t kSubModuleId           = 14;
    static const uint8_t kSubTime               = 15;
    static const uint8_t kSubFirmwareVersion    = 16;
    static const uint8_t kSubHardwareVersion    = 17;
    static const uint8_t kSubElectronicsVersion = 18;
    static const uint8_t kSubFirmwareValid      = 19;
};

#endif // SYSTEM_CONTROL_CLIENT_H
