/*
 * multi_turn_angle_control_client.hpp
 *
 *  Created on: May 5, 2017
 *      Author: Matthew Piccoli
 */

#ifndef PENN_ECOSYSTEM_HAL_COMMON_CPP_INC_MULTI_TURN_ANGLE_CONTROL_CLIENT_HPP_
#define PENN_ECOSYSTEM_HAL_COMMON_CPP_INC_MULTI_TURN_ANGLE_CONTROL_CLIENT_HPP_


#include "communication_interface.h"
#include "client_communication.hpp"

//TODO::Cleanup then include common_message_types and delete the below line
const uint8_t kTypeAngleMotorControl = 59;

class MultiTurnAngleControlClient: public ClientAbstract{
  public:
    MultiTurnAngleControlClient(uint8_t obj_idn):
      ClientAbstract(                   kTypeAngleMotorControl, obj_idn),
      ctrl_mode_(                       kTypeAngleMotorControl, obj_idn, kSubCtrlMode),
      ctrl_brake_(                      kTypeAngleMotorControl, obj_idn, kSubCtrlBrake),
      ctrl_coast_(                      kTypeAngleMotorControl, obj_idn, kSubCtrlCoast),
      ctrl_angle_(                      kTypeAngleMotorControl, obj_idn, kSubCtrlAngle),
      ctrl_velocity_(                   kTypeAngleMotorControl, obj_idn, kSubCtrlVelocity),
      angle_Kp_(                        kTypeAngleMotorControl, obj_idn, kSubAngleKp),
      angle_Ki_(                        kTypeAngleMotorControl, obj_idn, kSubAngleKi),
      angle_Kd_(                        kTypeAngleMotorControl, obj_idn, kSubAngleKd),
      timeout_(                         kTypeAngleMotorControl, obj_idn, kSubTimeout),
      filter_fc_(                       kTypeAngleMotorControl, obj_idn, kSubFilterFc),
      filter_fs_(                       kTypeAngleMotorControl, obj_idn, kSubFilterFs),
      obs_angular_displacement_(        kTypeAngleMotorControl, obj_idn, kSubObsAngularDisplacement),
      obs_angular_velocity_(            kTypeAngleMotorControl, obj_idn, kSubObsAngularVelocity),
      meter_per_rad_(                   kTypeAngleMotorControl, obj_idn, kSubMeterPerRad),
      ctrl_linear_displacement_(        kTypeAngleMotorControl, obj_idn, kSubCtrlLinearDisplacement),
      ctrl_linear_velocity_(            kTypeAngleMotorControl, obj_idn, kSubCtrlLinearVelocity),
      obs_linear_displacement_(         kTypeAngleMotorControl, obj_idn, kSubObsLinearDisplacement),
      obs_linear_velocity_(             kTypeAngleMotorControl, obj_idn, kSubObsLinearVelocity),
      angular_speed_max_(               kTypeAngleMotorControl, obj_idn, kSubAngularSpeedMax),
      trajectory_angular_displacement_( kTypeAngleMotorControl, obj_idn, kSubTrajectoryAngularDisplacement),
      trajectory_angular_velocity_(     kTypeAngleMotorControl, obj_idn, kSubTrajectoryAngularVelocity),
      trajectory_angular_acceleration_( kTypeAngleMotorControl, obj_idn, kSubTrajectoryAngularAcceleration),
      trajectory_duration_(             kTypeAngleMotorControl, obj_idn, kSubTrajectoryDuration),
      trajectory_linear_displacement_(  kTypeAngleMotorControl, obj_idn, kSubTrajectoryLinearDisplacement),
      trajectory_linear_velocity_(      kTypeAngleMotorControl, obj_idn, kSubTrajectoryLinearVelocity),
      trajectory_linear_acceleration_(  kTypeAngleMotorControl, obj_idn, kSubTrajectoryLinearAcceleration)
      {};

    // Client Entries
    // Control commands
    ClientEntry<uint8_t>    ctrl_mode_;
    ClientEntryVoid         ctrl_brake_;
    ClientEntryVoid         ctrl_coast_;
    ClientEntry<float>      ctrl_angle_;
    ClientEntry<float>      ctrl_velocity_;
    // Angle control
    ClientEntry<float>      angle_Kp_;
    ClientEntry<float>      angle_Ki_;
    ClientEntry<float>      angle_Kd_;
    // Timeout
    ClientEntry<float>      timeout_;
    // Filter
    ClientEntry<uint32_t>   filter_fc_;
    ClientEntry<uint32_t>   filter_fs_;
    // Angular values
    ClientEntry<float>      obs_angular_displacement_;
    ClientEntry<float>      obs_angular_velocity_;
    // Linear conversion
    ClientEntry<float>      meter_per_rad_;
    ClientEntry<float>      ctrl_linear_displacement_;
    ClientEntry<float>      ctrl_linear_velocity_;
    ClientEntry<float>      obs_linear_displacement_;
    ClientEntry<float>      obs_linear_velocity_;
    // Limits
    ClientEntry<float>      angular_speed_max_;
    // Trajectory
    ClientEntry<float>      trajectory_angular_displacement_;
    ClientEntry<float>      trajectory_angular_velocity_;
    ClientEntry<float>      trajectory_angular_acceleration_;
    ClientEntry<float>      trajectory_duration_;
    ClientEntry<float>      trajectory_linear_displacement_;
    ClientEntry<float>      trajectory_linear_velocity_;
    ClientEntry<float>      trajectory_linear_acceleration_;

    void ReadMsg(CommunicationInterface& com,
      uint8_t* rx_data, uint8_t rx_length)
    {
      static const uint8_t kEntryLength = kSubTrajectoryLinearAcceleration+1;
      ClientEntryAbstract* entry_array[kEntryLength] = {
        &ctrl_mode_,                        // 0
        &ctrl_brake_,                       // 1
        &ctrl_coast_,                       // 2
        &ctrl_angle_,                       // 3
        &ctrl_velocity_,                    // 4
        &angle_Kp_,                         // 5
        &angle_Ki_,                         // 6
        &angle_Kd_,                         // 7
        &timeout_,                          // 8
        &filter_fc_,                        // 9
        &filter_fs_,                        // 10
        &obs_angular_displacement_,         // 11
        &obs_angular_velocity_,             // 12
        &meter_per_rad_,                    // 13
        &ctrl_linear_displacement_,         // 14
        &ctrl_linear_velocity_,             // 15
        &obs_linear_displacement_,          // 16
        &obs_linear_velocity_,              // 17
        &angular_speed_max_,                // 18
        &trajectory_angular_displacement_,  // 19
        &trajectory_angular_velocity_,      // 20
        &trajectory_angular_acceleration_,  // 21
        &trajectory_duration_,              // 22
        &trajectory_linear_displacement_,   // 23
        &trajectory_linear_velocity_,       // 24
        &trajectory_linear_acceleration_    // 25
      };

      ParseMsg(rx_data, rx_length, entry_array, kEntryLength);
    }

  private:
    static const uint8_t kSubCtrlMode                         =  0;
    static const uint8_t kSubCtrlBrake                        =  1;
    static const uint8_t kSubCtrlCoast                        =  2;
    static const uint8_t kSubCtrlAngle                        =  3;
    static const uint8_t kSubCtrlVelocity                     =  4;
    static const uint8_t kSubAngleKp                          =  5;
    static const uint8_t kSubAngleKi                          =  6;
    static const uint8_t kSubAngleKd                          =  7;
    static const uint8_t kSubTimeout                          =  8;
    static const uint8_t kSubFilterFc                         =  9;
    static const uint8_t kSubFilterFs                         = 10;
    static const uint8_t kSubObsAngularDisplacement           = 11;
    static const uint8_t kSubObsAngularVelocity               = 12;
    static const uint8_t kSubMeterPerRad                      = 13;
    static const uint8_t kSubCtrlLinearDisplacement           = 14;
    static const uint8_t kSubCtrlLinearVelocity               = 15;
    static const uint8_t kSubObsLinearDisplacement            = 16;
    static const uint8_t kSubObsLinearVelocity                = 17;
    static const uint8_t kSubAngularSpeedMax                  = 18;
    static const uint8_t kSubTrajectoryAngularDisplacement    = 19;
    static const uint8_t kSubTrajectoryAngularVelocity        = 20;
    static const uint8_t kSubTrajectoryAngularAcceleration    = 21;
    static const uint8_t kSubTrajectoryDuration               = 22;
    static const uint8_t kSubTrajectoryLinearDisplacement     = 23;
    static const uint8_t kSubTrajectoryLinearVelocity         = 24;
    static const uint8_t kSubTrajectoryLinearAcceleration     = 25;
};



#endif /* PENN_ECOSYSTEM_HAL_COMMON_CPP_INC_MULTI_TURN_ANGLE_CONTROL_CLIENT_HPP_ */
