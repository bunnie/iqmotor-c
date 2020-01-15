#ifndef MTA_H
#define MTA_H

#include <stdbool.h>

#define kTypeAngleMotorControl 59

typedef enum {
  mta_void = 0,
  mta_uint8_t = 1,
  mta_float = 2
} mta_type;

typedef enum {
  kSubCtrlMode                         =  0,
  kSubCtrlBrake                        =  1,
  kSubCtrlCoast                        =  2,
  kSubCtrlAngle                        =  3,
  kSubCtrlVelocity                     =  4,
  kSubAngleKp                          =  5,
  kSubAngleKi                          =  6,
  kSubAngleKd                          =  7,
  kSubTimeout                          =  8,
  kSubCtrlPwm                          =  9,
  kSubCtrlVolts                        = 10,
  kSubObsAngularDisplacement           = 11,
  kSubObsAngularVelocity               = 12,
  kSubMeterPerRad                      = 13,
  kSubCtrlLinearDisplacement           = 14,
  kSubCtrlLinearVelocity               = 15,
  kSubObsLinearDisplacement            = 16,
  kSubObsLinearVelocity                = 17,
  kSubAngularSpeedMax                  = 18,
  kSubTrajectoryAngularDisplacement    = 19,
  kSubTrajectoryAngularVelocity        = 20,
  kSubTrajectoryAngularAcceleration    = 21,
  kSubTrajectoryDuration               = 22,
  kSubTrajectoryLinearDisplacement     = 23,
  kSubTrajectoryLinearVelocity         = 24,
  kSubTrajectoryLinearAcceleration     = 25
} mta_command;

typedef struct mta_storage {
  bool is_fresh;
  mta_command command;
  mta_type type;
  union {
    uint8_t c;
    float f;
  } data;
} mta_storage;

typedef struct mta_object {
  struct CommInterface_storage *com;
  uint8_t obj_idn;
  mta_storage data; // item for holding the current data
} mta_object;

// method protos
void mta_init(struct mta_object *mta, struct CommInterface_storage *com, uint8_t obj_idn);
void mta_get(struct mta_object *mta, mta_command cmd);
void mta_set(struct mta_object *mta, mta_command cmd); // data to set is in mta->
void mta_save(struct mta_object *mta, mta_command cmd);
void mta_Reply(struct mta_object *mta, uint8_t *data, uint8_t len, mta_command cmd);
void mta_get_reply(struct mta_object *mta); // reply is in mta->data.type, mta->data.data; all other fields ignored
bool mta_IsFresh(struct mta_object *mta);

#define kEntryLength  (kSubTrajectoryLinearAcceleration+1)

#endif
