//
// Created by clemens on 09.09.24.
//

#include "power_service.hpp"

#include <ulog.h>

#include <globals.hpp>

#include "board.h"

void PowerService::SetDriver(ChargerDriver* charger_driver) {
  charger_ = charger_driver;
}

bool PowerService::OnStart() {
  charger_configured_ = false;
  return true;
}

void PowerService::tick() {
  xbot::service::Lock lk{&mtx_};
  // Send the sensor values
  StartTransaction();
  auto status_str = GetChargerStatus();
  SendChargingStatus(status_str, strlen(status_str));
  SendBatteryVoltage(battery_volts);
  SendChargeVoltage(adapter_volts);
  SendChargeCurrent(charge_current);
  SendChargerEnabled(true);
  SendBatteryPercentage(etl::max(0.0f, etl::min(1.0f, battery_percent)));
  CommitTransaction();
}
void PowerService::charger_tick() {
  if (charger_ == nullptr) {
    ULOG_ARG_ERROR(&service_id_, "Charger is null!");
    return;
  }

  if (!charger_configured_) {
    // charger not configured, configure it
    if (charger_->init()) {
      // Set the currents low
      bool success = true;
      success &= charger_->setPreChargeCurrent(0.250f);
      success &= charger_->setTerminationCurrent(0.250f);
      success &= charger_->setChargingCurrent(robot->Power_GetDefaultChargeCurrent(), false);
      // Disable temperature sense, the battery doesnt have it
      success &= charger_->setTsEnabled(false);
      charger_configured_ = success;
    }

    if (charger_configured_) {
      ULOG_ARG_INFO(&service_id_, "Successfully Configured Charger");
    } else {
      ULOG_ARG_ERROR(&service_id_, "Unable to Configure Charger");
    }
  } else {
    xbot::service::Lock lk{&mtx_};
    // charger is configured, do monitoring
    bool success = true;
    {
      bool s = charger_->resetWatchdog();
      if (!s) {
        ULOG_ARG_WARNING(&service_id_, "Error Resetting Watchdog");
      }
      success &= s;
    }
    {
      bool s = charger_->readChargeCurrent(charge_current);
      if (!s) {
        ULOG_ARG_WARNING(&service_id_, "Error Reading Charge Current");
      }
      success &= s;
    }
    {
      bool s = charger_->readBatteryVoltage(battery_volts);
      if (!s) {
        ULOG_ARG_WARNING(&service_id_, "Error Reading Battery Voltage");
      }
      success &= s;
    }
    {
      bool s = charger_->readAdapterVoltage(adapter_volts);
      if (!s) {
        ULOG_ARG_WARNING(&service_id_, "Error Reading Adapter Voltage");
      }
      success &= s;
    }
    {
      bool s = charger_->readAdapterCurrent(adapter_current);
      if (!s) {
        ULOG_ARG_WARNING(&service_id_, "Error Reading Adapter Current");
      }
      success &= s;
    }

    if (success) {
      battery_percent = etl::max(0.0f, etl::min(1.0f, (battery_volts - robot->Power_GetDefaultBatteryEmptyVoltage()) /
                                                          (robot->Power_GetDefaultBatteryFullVoltage() -
                                                           robot->Power_GetDefaultBatteryEmptyVoltage())));
    }

    charger_status = charger_->getChargerStatus();

    if (!success || charger_status == CHARGER_STATUS::COMMS_ERROR) {
      // Error during comms or watchdog timer expired, reconfigure charger
      charger_configured_ = false;
      ULOG_ARG_ERROR(&service_id_, "Error during charging comms - reconfiguring");
    } else {
      if (battery_volts < robot->Power_GetAbsoluteMinVoltage()) {
        critical_count++;
        if (critical_count > 10) {
          palClearLine(LINE_HIGH_LEVEL_GLOBAL_EN);
        }
      } else {
        critical_count = 0;
        palSetLine(LINE_HIGH_LEVEL_GLOBAL_EN);
      }
    }
  }
}

void PowerService::OnChargingAllowedChanged(const uint8_t& new_value) {
  (void)new_value;
}
