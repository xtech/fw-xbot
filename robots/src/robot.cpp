#include "../include/robot.hpp"

#include "../include/xbot_robot.hpp"

Robot* GetRobot() {
  // TODO: This should look at the flash to determine the mower type.
  // For now, there's only one
  return new xBotRobot();
}
