//
// Created by clemens on 09.09.24.
//

#ifndef POWER_SERVICE_HPP
#define POWER_SERVICE_HPP

#include <ch.h>

#include <PowerServiceBase.hpp>
#include <drivers/charger/charger.hpp>
#include <xbot-service/Lock.hpp>

using namespace xbot::service;

class PowerService : public PowerServiceBase {
 public:
  explicit PowerService(uint16_t service_id) : PowerServiceBase(service_id, wa, sizeof(wa)) {
  }

  void SetDriver(ChargerDriver* charger_driver);

  [[nodiscard]] float GetChargeCurrent() {
    xbot::service::Lock lk{&mtx_};
    return charge_current;
  }

  [[nodiscard]] float GetAdapterVolts() {
    xbot::service::Lock lk{&mtx_};
    return adapter_volts;
  }

  [[nodiscard]] float GetBatteryVolts() {
    xbot::service::Lock lk{&mtx_};
    return battery_volts;
  }

  [[nodiscard]] float GetBatteryPercent() {
    xbot::service::Lock lk{&mtx_};
    return battery_percent;
  }

 protected:
  bool OnStart() override;

 private:
  MUTEX_DECL(mtx_);

  static constexpr auto CHARGE_STATUS_ERROR_STR = "Error";
  static constexpr auto CHARGE_STATUS_FAULT_STR = "Error (Fault)";
  static constexpr auto CHARGE_STATUS_NOT_FOUND_STR = "Charger Not Found";
  static constexpr auto CHARGE_STATUS_COMMS_ERROR_STR = "Charger Comms Error";
  static constexpr auto CHARGE_STATUS_NOT_CHARGING_STR = "Not Charging";
  static constexpr auto CHARGE_STATUS_PRE_CHARGE_STR = "Pre Charge";
  static constexpr auto CHARGE_STATUS_TRICKLE_STR = "Trickle Charge";
  static constexpr auto CHARGE_STATUS_CC_STR = "Fast Charge (CC)";
  static constexpr auto CHARGE_STATUS_CV_STR = "Taper Charge (CV)";
  static constexpr auto CHARGE_STATUS_TOP_OFF_STR = "Top Off";
  static constexpr auto CHARGE_STATUS_DONE_STR = "Done";
  static constexpr auto CHARGE_STATUS_UNKNOWN_STR = "Unknown State";

  void tick();
  void charger_tick();
  ServiceSchedule tick_schedule_{*this, 1'000'000, XBOT_FUNCTION_FOR_METHOD(PowerService, &PowerService::tick, this)};
  Schedule charger_managed_schedule_{scheduler_, true, 1'000'000,
                                     XBOT_FUNCTION_FOR_METHOD(PowerService, &PowerService::charger_tick, this)};

  bool charger_configured_ = false;
  float charge_current = 0;
  float adapter_volts = 0;
  float battery_volts = 0;
  float battery_percent = 0;
  int critical_count = 0;
  CHARGER_STATUS charger_status = CHARGER_STATUS::COMMS_ERROR;
  ChargerDriver* charger_ = nullptr;
  THD_WORKING_AREA(wa, 1500){};

 protected:
  void OnChargingAllowedChanged(const uint8_t& new_value) override;
};

#endif  // POWER_SERVICE_HPP
