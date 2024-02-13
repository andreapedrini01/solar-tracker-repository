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
Once the solar tracker structure is connected to power and the microcontroller is linked to the computer containing the project's code, we compile and launch the code using Code Composer Studio. Then, it will be possible to observe the current values received by each of the four photoresistors.

The following points denote the most important code sections that allow the visualization of data and configuration of the necessary components to enable the movement of the structure to make this structure an effective solar tracker.

#### Initialization of hardware and motors 
Given that the microcontroller we are using is an MSP432P401R and it is highly configurable, a proper initialization of its peripherals is important for optimal operation.
```c
void _hwInit()
{
    // Halting WDT and disabling master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();
    init_motors();
    _adcInit();
}
```
The function above is responsible for initializing the necessary hardware for the system's operation, such as the stepper motor, the ADC (Analog-to-Digital Converter), and any other required configurations.

```c
void init_motors() {
    //base motor
    init_baseStepper();

    //top motor
    init_topStepper();
}
```
The movement of the solar tracker is based on biaxial motion, so we initialize the base and top motors, which are responsible for horizontal and vertical movements, respectively.

We observe the initialization of the base motor. For the vertical movement, the motor is initialized in a similar manner.

```c
void init_baseStepper() {
    // Initialize the port and pin
    MAP_WDT_A_holdTimer();

    // Configure the pins as outputs
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, BASE_STEP_PIN | BASE_DIR_PIN);

   // Set pins to initial low level
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, BASE_STEP_PIN | BASE_DIR_PIN);
}
```

#### Initialization of ADC

```c
void _adcInit(){
        /* Configures Pin 6.0 and 4.4 as ADC input */
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

        /* Initializing ADC (ADCOSC/64/8) */
        ADC14_enableModule();
        ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

...

}

```
Extremely important function as it initializes the ADC (Analog-to-Digital Converter) and configures the pins 5.2, 5.1, 5.0 and 5.4 as ADC inputs without which it would be impossible to directly read the values of the photoresistors in digital form. The photoresistors provide an analog output that varies depending on the intensity of the incident light, and this analog signal must be converted to digital so that the microcontroller can interpret it.

#### Read and Movement Functions
The functions responsible for controlling both motors to move or not are **readAndMove()**, **horMov()**, and **verMov()**. Starting with the latter two we can say that they function almost analogously. The differences between them are mainly based on limitations of the movement that we have had to implement to prevent damage to the components, in the calculation of differences in sensor values, and finally in the obvious difference in the direction of movement.

We begin analyzing the operation of **horMov()**.

```c
void horMov() {
    int horizontalSteps = 0;
       int diff1 = 0;
       int diff1_1 = 0;

      /* Store ADC14 conversion results */
           resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
           resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
           resultsBuffer[2] = ADC14_getResult(ADC_MEM2);
           resultsBuffer[3] = ADC14_getResult(ADC_MEM3);

             printf("PR0: %5d\n", resultsBuffer[0]);
             printf("PR1: %5d\n", resultsBuffer[1]);
             printf("PR2: %5d\n", resultsBuffer[2]);
             printf("PR3: %5d\n\n", resultsBuffer[3]);
...
```
First, the values of the four light sensors are read and stored in the **resultsBuffer** array. These values represent the intensity of light received by each sensor. Subsequently, the average intensity of light is calculated from the sensor values. This average is used later to make decisions about the movement of the panel.

```c
...
int avgIntensity = 0;
       int i=0;
       for (i = 0; i < NUM_SENSORS; i++) {
           avgIntensity += resultsBuffer[i];
       }
       avgIntensity /= NUM_SENSORS;
...
```
To detect changes in the intensity of incident light, the differences between the values of horizontally opposite sensors are calculated to determine if there has been a significant change in this intensity.
```c
...
if (avgIntensity > LIGHT_THRESHOLD) {
           diff1 = resultsBuffer[0] - resultsBuffer[1];
           diff1_1 = resultsBuffer[3] - resultsBuffer[2];
           printf("diff1 = %d\n", diff1);
           printf("diff1_1 = %d\n", diff1_1);
...
```
If a significant change in the intensity of light is detected, this change is mapped to motor steps. This involves converting the intensity difference into a specific number of steps that the motor must perform.
```c
...
if (abs(diff1) >= VALUE_CHANGE) {
                      horizontalSteps = map(diff1, -16383, 16383, -10000, 10000);
           } else if (abs(diff1_1) >= VALUE_CHANGE)
                      horizontalSteps = map(diff1_1, -16383, 16383, -10000, 10000);
...
```
Limits are applied to the motor steps to ensure safe movement within the established range. Then, pulses are sent to the base motor to move the solar panel in the appropriate horizontal direction.
```c
...
           if (horizontalSteps != 0) {
              horizontalSteps = limitSteps(base_position,horizontalSteps);
              printf("horizontalSteps after limiting = %d\n", horizontalSteps);
              base_position += horizontalSteps;
              moveBase(horizontalSteps);
          }
       }
}
```

#### Step restriction
*Post a photo of the Solar Tracker and explain why it would be harmful for the arm to move in any direction.*

#### Mapping of input values



## Team
| Members        | Mail |
|--|--|
| Andrea Pedrini | andrea.pedrini@studenti.unitn.it |
| Irina Voloshina | irina.voloshina@studenti.unitn.it |
| Riccardo Stiz | riccardo.stiz@studenti.unitn.it |
| Francheska Paz Contreras Delgado | fp.contrerasdelgado@studenti.unitn.it |
