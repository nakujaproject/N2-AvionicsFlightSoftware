# N2-flight-software
## Introduction
This repository contains code used for flight software for Nakuja Rocket N2. The following description provides a detailed
description of program flow, project structure, library dependencies and/or any other necessary documentation needed to run this 
code successfully.

## Project structure
***

```asm
├── data
│   └── flightdata.txt
├── include
│   ├── checkState.h
│   ├── defs.h
│   ├── functions.h
│   ├── kalmanfilter.h
│   ├── logdata.h
│   ├── readsensors.h
│   └── transmitwifi.h
├── lib
│   └── README
├── LICENSE
├── platformio.ini
├── README.md
├── src
│   └── main.cpp
└── test
    └── README
```

### 1. Folders Description
| Folder  | Description   |
|---|---|
|  include | Contains header files   |
|  lib | Project libraries|
|src| Contains main.cpp source file run by the flight computer   |
|test| Contains unit test files    |

### 2. Files Description
| File | Description |
| ---------| ----------- |
|defs.h  | Contains constants declaration |
|functions.h| Contains general utility functions and prototype function declarations |
|logdata.h | code for SD card mounting and logging |
|checkState.h| Contains state machine logic |
|kalmanfilter.h | Contains kalman filter for accelerometer values |
|readsensors.h | Contains code for sensors on board including the MPU,BMP,GPS |
|transmitwifi.h | Contains WiFi set-up and MQTT code |


## Flight Logic 
The flight program is structured as a state machine with the following states: 
| State  | Description  | Waiting for event |
|---|---| -----|
| 0  | Pre-Flight Ground    | altitude >= 10m AGL  |
| 1  | Powered Flight      |   |
| 2  | Coasting   |  velocity < 0  to detect apogee  |
| 3  | Apogee   |  10m below apogee to deploy parachute  |
| 4  | Chute Descent| altitude <= 10m AGL|
| 5  | Post-flight Ground    |  No more events  |

## Safe-mode
Proposed safe mode conditions to prevent premature misfires of the parachute ejection system:
1. Ejection system pins are disabled 
2. State transitions are locked
3. State is maintained at SAFE_MODE-PREFLIGHT

## Flight Mode
In the flight-mode:
1. The parachute ejection system is enabled- any pin being used in the ejection system is enabled
2. State transition flag is enabled 


#### State diagram
![state-diagram](./docs/state.png)


