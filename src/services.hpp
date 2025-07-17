#ifndef SERVICES_HPP
#define SERVICES_HPP

#include "services/diff_drive_service/diff_drive_service.hpp"
#include "services/emergency_service/emergency_service.hpp"
#include "services/imu_service/imu_service.hpp"
#include "services/input_service/input_service.hpp"
#include "services/power_service/power_service.hpp"

extern EmergencyService emergency_service;
extern DiffDriveService diff_drive;
extern ImuService imu_service;
extern PowerService power_service;
extern InputService input_service;

void StartServices();

#endif  // SERVICES_HPP
