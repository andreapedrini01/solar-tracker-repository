# Solar tracker

| Members        | Mail |
|--|--|
| Andrea Pedrini | andrea.pedrini@studenti.unitn.it |
| Irina Voloshina | irina.voloshina@studenti.unitn.it |
| Riccardo Stiz | riccardo.stiz@studenti.unitn.it |
| Francheska Paz Contreras Delgado | fp.contrerasdelgado@studenti.unitn.it |

## Table of contents
- [About this project](#about-this-project)
- [Requirements](#requirements)
  - [Hardware](#hardware)
  - [Software](#software)
- [Folder structure](#folder-structure)
- [Functionalities explanation](#functionalities-explanation)
- [Getting started](#getting-started)
 

## About the project
The idea of this project not only arises from the challenge of how to take advantage of the benefits and features of a microcontroller like the MSP432P401R, but also from how to provide a solution to a real-world problem.
For this reason, the implementation of a solar tracker was chosen. In this way, we could enhance the efficiency of solar panels by enabling active tracking of the sun's movement throughout the day.
Our goal for this solar tracker project is to implement it in the simplest way possible, leveraging the inherent functionality of the MSP432P401R and avoiding the use of other types of microcontrollers. All of this within a structure capable of providing support and stability for the photoresistors, solar panels, and transmitters that will enable the movement of the entire structure.

- [PowerPoint presentation]()
- Video demostration <...>

## Requirements
#### Hardware
For this project we use:

- Texas Instruments MSP432P401R microcontroller
- 2 Gearboxes
- 2 Stepper motors
- 4 Photoresistor
- Current-limiting resistor
- Jumper wires
- Solar panel
- Base and columns to assemble everything as a whole
  
#### Software
- Code Composer Studio
- Solidworks (For the design of the containing structure)

## Folder structure
.
.
.

## Functionalities explanation
Once the solar tracker structure is connected to power and the microcontroller is linked to the computer containing the project code, we compile and launch the code using Code Composer Studio.

In the Code Composer Studio console, we can observe the current values received by each of the four photoresistors. Initially, the average light intensity received by the photoresistors is calculated. If this average surpasses a previously defined luminous threshold (LIGHT_THRESHOLD), the calculations for the horizontal movement of the motor are initiated.

(*This part is subject to change*)
The horizontal movement relies on the stepper motor located at the base and the difference between the values read by photoresistors [0] and [1]. If this difference is equal to or greater than another previously defined threshold (VALUE_CHANGE), the steps that the motor must take are calculated clockwise or counterclockwise and within defined limits (to prevent damage to other electrical components).

After all the preceding verifications and controls, the horizontal movement of the structure is executed until reaching the optimal position for light intensity.

Once the horizontal movement of the arm is completed, we proceed with the vertical movement (if required) in a similar manner, but taking into consideration the variation of the values ​​of the photoresistors [?] and [?].

## Getting started
.
.
.
