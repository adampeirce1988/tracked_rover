# Tracked Rover Project

A PlatformIO-based rover software project scaffold for tracked rover control.

## Structure

- `src/main.cpp` - application entry point and loop.
- `include/` - project headers.
- `lib/motor_control/` - motor control module.
- `platformio.ini` - build configuration.

## Build

Run in the project directory:

```sh
platformio run
```

Or monitor serial output:

```sh
platformio device monitor
```

## Boards

This scaffold includes two environments:
- `esp32dev` (default)
- `uno`

Change the environment in `platformio.ini` or run:

```sh
platformio run -e uno
```
