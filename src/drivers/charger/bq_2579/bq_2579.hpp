//
// Created by clemens on 03.04.25.
//

#ifndef BQ_2579_HPP
#define BQ_2579_HPP

#include <drivers/charger/charger.hpp>

class BQ2579 : public ChargerDriver {
 public:
  ~BQ2579() override;
  bool setChargingCurrent(float current_amps, bool overwrite_hardware_limit) override;
  bool setPreChargeCurrent(float current_amps) override;
  bool setTerminationCurrent(float current_amps) override;
  CHARGER_STATUS getChargerStatus() override;
  bool init() override;
  bool resetWatchdog() override;
  bool setTsEnabled(bool enabled) override;
  bool readChargeCurrent(float &result) override;
  bool readAdapterVoltage(float &result) override;
  bool readBatteryVoltage(float &result) override;
  bool readSystemVoltage(float &result);
  bool readAdapterCurrent(float &result) override;

 private:
  static constexpr uint8_t DEVICE_ADDRESS = 0x6B;
  static constexpr uint8_t REG_Charge_Voltage_Limit = 0x01;
  static constexpr uint8_t REG_Charge_Current_Limit = 0x03;
  static constexpr uint8_t REG_IBUS_ADC = 0x31;
  static constexpr uint8_t REG_IBAT_ADC = 0x33;
  static constexpr uint8_t REG_VBUS_ADC = 0x35;
  static constexpr uint8_t REG_VBAT_ADC = 0x3B;
  static constexpr uint8_t REG_VSYS_ADC = 0x3D;
  static constexpr uint8_t REG_ADC_Control = 0x2E;
  static constexpr uint8_t REG_Termination_Control = 0x09;
  static constexpr uint8_t REG_NTC_Control_1 = 0x18;
  static constexpr uint8_t REG_Charger_Control_1 = 0x10;
  static constexpr uint8_t REG_Charger_Control_5 = 0x14;
  static constexpr uint8_t REG_Charger_Status_1 = 0x1C;
  static constexpr uint8_t REG_FAULT_Status_0 = 0x20;
  static constexpr uint8_t REG_FAULT_Status_1 = 0x21;

  bool readRegister(uint8_t reg, uint8_t &result);
  bool readRegister(uint8_t reg, uint16_t &result);
  bool writeRegister8(uint8_t reg, uint8_t value);
  bool writeRegister16(uint8_t reg, uint16_t value);
};

#endif  // BQ_2579_HPP
