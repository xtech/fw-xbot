cmake_minimum_required(VERSION 3.22)

include(robots/CMakeLists.txt)

set(CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cfg)
set(BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boards/XCORE)

# Set custom port path and extended service classes
set(XBOT_CUSTOM_PORT_PATH ${CMAKE_SOURCE_DIR}/portable/xbot)
set(XBOT_SERVICE_EXT "services/service_ext.hpp")

# Set the project name
set(CMAKE_PROJECT_NAME xbot-firmware)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")


# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)


# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})
# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
)

target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -Wall -Wextra -Werror)


# Add lib subdirectory
add_subdirectory(ext)
target_compile_definitions(ulog PUBLIC ULOG_ENABLED)

# Add mongoose (here because the mongoose folder will be overwritten by the wizard)
add_library(mongoose mongoose/mongoose.c mongoose/mongoose_impl.c mongoose/mongoose_fs.c)
target_include_directories(mongoose PUBLIC mongoose)
target_link_libraries(mongoose PUBLIC ChibiOS)
target_compile_definitions(mongoose PUBLIC MG_ARCH=MG_ARCH_CUSTOM)


# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        boards/XCORE/board_utils.cpp
        src/main.cpp
        src/globals.cpp
        src/heartbeat.c
        src/id_eeprom.c
        src/boot_service_discovery.cpp
        src/json_stream.cpp
        src/services.cpp
        src/status_led.c
        # LittleFS helpers
        src/filesystem/file.cpp
        src/filesystem/filesystem.cpp
        # Service Implementations
        src/services/imu_service/imu_service.cpp
        src/services/power_service/power_service.cpp
        src/services/emergency_service/emergency_service.cpp
        src/services/diff_drive_service/diff_drive_service.cpp
        src/services/input_service/input_service.cpp
        # BQ25679 driver
        src/drivers/charger/bq_2579/bq_2579.cpp
        # PWM motor driver
        src/drivers/motor/pwm/pwm_motor_driver.cpp
        # Input driver
        src/drivers/input/input_driver.cpp
        src/drivers/input/gpio_input_driver.cpp
        $<$<CONFIG:Debug>:src/drivers/input/simulated_input_driver.cpp>
        # Raw driver debug interface
        src/debug/debug_tcp_interface.cpp
        src/debug/debug_udp_interface.cpp
        src/debug/debuggable_driver.cpp
        robots/src/robot.cpp
        ${PLATFORM_SOURCES}
        # API
        src/api.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined include paths
        src
        robots/include
        services
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined symbols
        timegm=mktime
        ROBOT_PLATFORM=${ROBOT_PLATFORM}
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
        # Add user defined libraries
        ChibiOS
        LSM6DS3TR
        etl::etl
        LittleFS
        lwjson
        mongoose
)

target_add_service(${CMAKE_PROJECT_NAME} ImuService ${CMAKE_CURRENT_SOURCE_DIR}/services/imu_service.json)
target_add_service(${CMAKE_PROJECT_NAME} PowerService ${CMAKE_CURRENT_SOURCE_DIR}/services/power_service.json)
target_add_service(${CMAKE_PROJECT_NAME} EmergencyService ${CMAKE_CURRENT_SOURCE_DIR}/services/emergency_service.json)
target_add_service(${CMAKE_PROJECT_NAME} DiffDriveService ${CMAKE_CURRENT_SOURCE_DIR}/services/diff_drive_service.json)
target_add_service(${CMAKE_PROJECT_NAME} InputService ${CMAKE_CURRENT_SOURCE_DIR}/services/input_service.json)

set_target_properties(${CMAKE_PROJECT_NAME}
        PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
        COMMAND ${CMAKE_OBJCOPY} -Oihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
)

add_custom_target(upload
        COMMAND docker run --rm --network=host -v ${CMAKE_BINARY_DIR}:/workdir ghcr.io/xtech/fw-xcore-boot:latest -i tap0 upload /workdir/${CMAKE_PROJECT_NAME}.bin
        DEPENDS ${CMAKE_PROJECT_NAME}
)
