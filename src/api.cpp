//
// Created by clemens on 24.06.25.
//

#include "api.hpp"
// clang-format off
#include "ch.h"
#include "hal.h"
// clang-format on

#include "mongoose_glue.h"
#include "services.hpp"

static THD_WORKING_AREA(waRestApi, 4096);

static void RestApiThreadFunc(void *) {
  mongoose_init();
  static systime_t last_ui_refresh = 0;
  while (1) {
    mongoose_poll();

    // Refresh UI every second
    if (chVTTimeElapsedSinceX(last_ui_refresh) > TIME_S2I(1)) {
      glue_update_state();
    }
  }
}

void InitRestAPI() {
  // Create a multicast sender thread
  thread_t *threadPointer = chThdCreateStatic(waRestApi, sizeof(waRestApi), LOWPRIO, RestApiThreadFunc, NULL);
  threadPointer->name = "REST API";
}

void glue_get_state(struct state *data) {
  static systime_t last_request = 0;
  static state state{};
  if (chVTTimeElapsedSinceX(last_request) < TIME_MS2I(100)) {
    // Don't allow spamming the API
    *data = state;
    return;
  }
  last_request = chVTGetSystemTimeX();

  // We'll set ros_online to false for now since it's not clear how to determine this
  state.ros_online = false;

  state.adapter_current = power_service.GetAdapterCurrent();
  state.battery_current = power_service.GetChargeCurrent();
  state.adapter_volts = power_service.GetAdapterVolts();
  state.battery_volts = power_service.GetBatteryVolts();
  state.battery_level = static_cast<int>(power_service.GetBatteryPercent() * 100);
  strncpy(state.charger_state, power_service.GetChargerStatus(), sizeof(state.charger_state));
  *data = state;
}

static struct power_service_settings s_power_service_settings = {12, 12.5, 16};
void glue_get_power_service_settings(struct power_service_settings *data) {
  *data = s_power_service_settings;  // Sync with your device
}
void glue_set_power_service_settings(struct power_service_settings *data) {
  s_power_service_settings = *data;  // Sync with your device
}

void glue_get_imu(struct imu *data) {
  double axes[9];
  imu_service.GetAxes(axes);
  data->ax = axes[0];
  data->ay = axes[1];
  data->az = axes[2];
  data->gx = axes[3];
  data->gy = axes[4];
  data->gz = axes[5];
}
