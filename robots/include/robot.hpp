#ifndef ROBOT_HPP
#define ROBOT_HPP

#include <hal.h>

#include <debug/debug_tcp_interface.hpp>

class Robot {
 public:
  virtual void InitPlatform() = 0;
  virtual bool IsHardwareSupported() = 0;

  virtual bool NeedsService(uint16_t id) {
    (void)id;
    return true;
  }

  virtual UARTDriver* GPS_GetUartPort() {
    // If nothing defined, we require a user setting.
    return nullptr;
  }

  /**
   * Return the default battery full voltage (i.e. this is considered 100% battery)
   */
  virtual float Power_GetDefaultBatteryFullVoltage() = 0;

  /**
   * Return the default battery empty voltage (i.e. this is considered 0% battery)
   */
  virtual float Power_GetDefaultBatteryEmptyVoltage() = 0;

  /**
   * Return the charging current for this robot
   */
  virtual float Power_GetDefaultChargeCurrent() = 0;

  /**
   * Return the minimum voltage before shutting down as much as possible
   */
  virtual float Power_GetAbsoluteMinVoltage() = 0;
};

Robot* GetRobot();

#endif  // ROBOT_HPP
