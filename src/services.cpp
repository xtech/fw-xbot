#include "services.hpp"

#include <service_ids.h>

#include "drivers/input/gpio_input_driver.hpp"
#ifdef DEBUG_BUILD
#include "drivers/input/simulated_input_driver.hpp"
#endif
#include "globals.hpp"

EmergencyService emergency_service{xbot::service_ids::EMERGENCY};
DiffDriveService diff_drive{xbot::service_ids::DIFF_DRIVE};
ImuService imu_service{xbot::service_ids::IMU};
PowerService power_service{xbot::service_ids::POWER};
InputService input_service{xbot::service_ids::INPUT};

void StartServices() {
#define START_IF_NEEDED(service, id)                \
  if (robot->NeedsService(xbot::service_ids::id)) { \
    service.start();                                \
  }

  if (robot->NeedsService(xbot::service_ids::INPUT)) {
    input_service.RegisterInputDriver("gpio", new GpioInputDriver{});
#ifdef DEBUG_BUILD
    input_service.RegisterInputDriver("simulated", new SimulatedInputDriver{});
#endif
  }

  START_IF_NEEDED(emergency_service, EMERGENCY)
  START_IF_NEEDED(imu_service, IMU)
  START_IF_NEEDED(power_service, POWER)
  START_IF_NEEDED(diff_drive, DIFF_DRIVE)
  START_IF_NEEDED(input_service, INPUT)
}
