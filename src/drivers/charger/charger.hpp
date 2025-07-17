//
// Created by clemens on 02.04.25.
//

#ifndef CHARGER_HPP
#define CHARGER_HPP

#include <hal.h>

enum class CHARGER_STATUS : uint8_t {

  NOT_CHARGING,
  TRICKLE,
  PRE_CHARGE,
  CC,
  CV,
  TOP_OFF,
  DONE,
  FAULT,
  COMMS_ERROR,
  UNKNOWN
};

class ChargerDriver {
 protected:
  I2CDriver *i2c_driver_ = nullptr;

 public:
  virtual ~ChargerDriver() = default;
  virtual bool setChargingCurrent(float current_amps, bool overwrite_hardware_limit) = 0;
  virtual bool setPreChargeCurrent(float current_amps) = 0;
  virtual bool setTerminationCurrent(float current_amps) = 0;
  virtual CHARGER_STATUS getChargerStatus() = 0;
  virtual bool init() = 0;
  virtual bool resetWatchdog() = 0;
  virtual bool setTsEnabled(bool enabled) = 0;
  virtual bool readChargeCurrent(float &result) = 0;
  virtual bool readAdapterVoltage(float &result) = 0;
  virtual bool readBatteryVoltage(float &result) = 0;

  void setI2C(I2CDriver *i2c) {
    i2c_driver_ = i2c;
  }
};

#endif  // CHARGER_HPP
