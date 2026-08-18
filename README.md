# Trator GPS Firmware

Firmware for the tractor GPS controller (ESP32) — in-development.

## Project Overview

This repository contains the ESP32 firmware for the "trator_gps" project. It uses the ESP-IDF build system (CMake) and is organized with a `main/` application, a set of tasks under `main/tasks/`, and additional components under `components/` and `managed_components/`.

The firmware implements the basic project structure, a task-oriented design, and a state machine task scaffold located at `main/tasks/state_machine_task.c`.

## What's Done (so far)

- Project scaffold: top-level `CMakeLists.txt`, `sdkconfig` files and working `build/` output are present.
- Application entry and organization: `main/` contains `main.c` and related component directories.
- Tasks: `main/tasks/` contains task implementations and the state machine task (`state_machine_task.c`).
- Components: local `components/` and `managed_components/` 
- Initial Tests were successful in the field with a 50 cm error average.
- Prototype validated

## Things to fix

- Debug and fix error in which the red led normal mode accumulated the distance delta.
- Add one more entry to the web interface with total session plantings and current file plantings.
- Reset the current file plantings on mode switching.

## Next Steps

- Further test the system in an open field with the RTK Base setup
- Improve the system to further decrease the localization error
- Final connection and test with the machine

## Repository Structure (high-level)

- `CMakeLists.txt` — top-level CMake config for ESP-IDF
- `sdkconfig`, `sdkconfig.ci`, `sdkconfig.old` — build configuration files
- `build/` — build artifacts (auto-generated)
- `main/` — main application; contains `CMakeLists.txt`, `main.c`, `tasks/`, and component code
- `components/` — local components
- `managed_components/` — vendor-managed components (e.g., `espressif__cjson`)

## Build & Flash (development)

Prerequisites:

- Install and configure ESP-IDF and toolchain for your platform. Follow the official ESP-IDF setup instructions for Windows/PowerShell.

Typical build and flash workflow (run inside an ESP-IDF-enabled shell or PowerShell with environment set):

```powershell
idf.py build
idf.py -p <YOUR_SERIAL_PORT> flash monitor
```

Replace `<YOUR_SERIAL_PORT>` with the appropriate COM port.

If you use other ESP-IDF helper scripts or a specific target you can run `idf.py set-target esp32` before building.

## Configuration

- `sdkconfig` controls build-time configuration. To change options run `idf.py menuconfig` from the project root.

## Testing & Logs

- Use the serial monitor (via `idf.py monitor`) to view log output from the device.
- Unit tests are not yet included in this repository; consider adding component/unit tests using the ESP-IDF Unity framework.

## Next Steps / To Do

- Complete and document the `state_machine_task` implementation (current file scaffold present).
- Add higher-level documentation for each task/component describing responsibilities and interfaces.
- Add CI for builds and automated tests (optional: `sdkconfig.ci` present as starting point).
- Add flashing and recovery instructions for field use.

## Contributing

- Create an issue describing the feature or bug.
- Open a pull request with changes; include build notes and tested targets.


