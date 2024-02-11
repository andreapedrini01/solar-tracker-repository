# Solar tracker

## Table of contents
- [About the project](#about-the-project)
- [Requirements](#requirements)
  - [Hardware](#hardware)
  - [Software](#software)
- [Installation](#installation)
- [Folder structure](#folder-structure)
- [Getting started](#getting-started)
- [Functionalities explanation](#functionalities-explanation)
- [Team](#team)

## About the project
The idea of this project not only arises from the challenge of how to take advantage of the benefits and features of a microcontroller like the MSP432P401R, but also from how to provide a solution to a real-world problem.
For this reason, the implementation of a solar tracker was chosen. In this way, we could enhance the efficiency of solar panels by enabling active tracking of the sun's movement throughout the day.
Our goal for this solar tracker project is to implement it in the simplest way possible, leveraging the inherent functionality of the MSP432P401R and avoiding the use of other types of microcontrollers. All of this within a structure capable of providing support and stability for the photoresistors, solar panels, and transmitters that will enable the movement of the entire structure.

- [PowerPoint presentation](https://docs.google.com/presentation/d/1o80_3NKI8f1lPXFbKVqxCV8jqO51_DSNfxD5pUymfhE/edit?usp=sharing)
- Video demostration <...>

## Requirements
#### Hardware
The hardware required for this project is listed as follows:
- Texas Instruments MSP432P401R microcontroller
- 2 Gearboxes
- 2 Stepper motors
- 4 Photoresistor
- Current-limiting resistor
- Jumper wires
- Solar panel
- Base and columns to assemble everything as a whole
  
#### Software
For this project, the following software is needed:
- Code Composer Studio as and IDE
- Solidworks (For the design of the containing structure)
  
### Installation
Clone the repository
```
git clone git@github.com:AirinLavis/iot-proj-es
```

## Folder structure
```
├── README.md
├── SOLIDWORKS
│   ├── 17HE15-1504S IMAGE.png
│   ├── 17HE15-1504S.STEP
│   ├── ARM IMAGE.png
│   ├── ARM.SLDPRT
│   ├── BRACKET IMAGE.png
│   ├── BRACKET.SLDPRT
│   ├── GEARBOX IMAGE.png
│   ├── GEARBOX MG17-G100.STEP
│   ├── LINK IMAGE.png
│   └── LINK.SLDPRT
└── Solar_Tracker
    ├── LcdDriver
    │   ├── Crystalfontz128x128_ST7735.c
    │   ├── Crystalfontz128x128_ST7735.h
    │   ├── HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.c
    │   └── HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h
    ├── StepperLib
    │   ├── base_stepper.c
    │   ├── base_stepper.h
    │   ├── top_stepper.c
    │   └── top_stepper.h
    ├── main.c
    ├── msp432p401r.cmd
    ├── startup_msp432p401r_ccs.c
    ├── system_msp432p401r.c
    └── targetConfigs
        └── readme.txt
```
## Getting started
1. Open the project in Code Composer Studio.
2. On the Arm Compiler options add the source directory:
   > *simplelink_msp432_sdk_3_40_01_02/*
3. On the Arm Linker options, add the following path:
   > *[...]/simplelink_msp432p4_sdk_3_40_01_02/source/ti/devices/msp432p4xx/driverlib/ccs/msp432p4xx_driverlib.lib*
4. Build and run the project. In the Code Composer Studio console, the values detected by the photoresistors should be visible.

## Functionalities explanation
Once the solar tracker structure is connected to power and the microcontroller is linked to the computer containing the project code, we compile and launch the code using Code Composer Studio.

In the Code Composer Studio console, we can observe the current values received by each of the four photoresistors. Initially, the average light intensity received by the photoresistors is calculated. If this average surpasses a previously defined luminous threshold (LIGHT_THRESHOLD), the calculations for the horizontal movement of the motor are initiated.

(*This part is subject to change*)
The horizontal movement relies on the stepper motor located at the base and the difference between the values read by photoresistors [0] and [1]. If this difference is equal to or greater than another previously defined threshold (VALUE_CHANGE), the steps that the motor must take are calculated clockwise or counterclockwise and within defined limits (to prevent damage to other electrical components).

After all the preceding verifications and controls, the horizontal movement of the structure is executed until reaching the optimal position for light intensity.

Once the horizontal movement of the arm is completed, we proceed with the vertical movement (if required) in a similar manner, but taking into consideration the variation of the values ​​of the photoresistors [?] and [?].

## Team
| Members        | Mail |
|--|--|
| Andrea Pedrini | andrea.pedrini@studenti.unitn.it |
| Irina Voloshina | irina.voloshina@studenti.unitn.it |
| Riccardo Stiz | riccardo.stiz@studenti.unitn.it |
| Francheska Paz Contreras Delgado | fp.contrerasdelgado@studenti.unitn.it |
