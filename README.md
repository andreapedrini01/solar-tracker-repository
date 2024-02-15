# Solar tracker
<img src="https://www.soleosenergy.com/wp-content/uploads/2023/12/Double-axis-Solar-Trackers.png" align="right" width="180">

## Table of contents
- [About the project](#about-the-project)
- [Requirements](#requirements)
  - [Hardware](#hardware)
  - [Software](#software)
- [Installation](#installation)
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
<img src="https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/zephyr/_images/msp_exp432p401r_launchxl.jpg" align="right" width="180">
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
## Getting started
1. Open the project in Code Composer Studio.
2. On the Arm Compiler options add the source directory:
   > *simplelink_msp432_sdk_3_40_01_02/*
3. On the Arm Linker options, add the following path:
   > *[...]/simplelink_msp432p4_sdk_3_40_01_02/source/ti/devices/msp432p4xx/driverlib/ccs/msp432p4xx_driverlib.lib*
4. Build and run the project.

## Functionalities explanation
Once the solar tracker structure is connected to power and the microcontroller is linked to the computer containing the project's code, we compile and launch the code using Code Composer Studio.

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
`adcInit()` is an extremely important function as it initializes the ADC (Analog-to-Digital Converter) and configures the pins 5.2, 5.1, 5.0 and 5.4 of the microcontroller as ADC inputs without which it would be impossible to directly read the values of the photoresistors in digital form. The photoresistors provide an analog output that varies depending on the intensity of the incident light, and this analog signal must be converted to digital so that the microcontroller can interpret it.

#### Mapping of input values
The `map` function takes a value `x` and maps it from a range specified by the input (`in_min` to `in_max`) to a range specified by the output (`out_min` to `out_max`). The precision of the result can be specified through the `precision` parameter.

```c
int map(int x, int in_min, int in_max, int out_min, int out_max, int precision){
    int top_part = (x - in_min) * (out_max - out_min);
    int bottom_part = in_max - in_min;
    return  (top_part / bottom_part) + out_min;
}
```

 The `map` function allows mapping a value read from an analog sensor to a more manageable range. In our projects, it enables us to convert readings from photoresistors, which range from 0 to 16383 (2^14 - 1), into a range between 0 and 1023 (2^10 - 1), typical for analog values. This simplifies data management by adjusting the sensor's input range to match the processing and visualization capabilities of the system.

```c
int scaleReading(reading) {
    return map(reading, 0, 16383, 0, 1023, 1);
}
```

We also use the `map` function to map the obtained value into the range of arm movement.

```c
    map(diff1, -1023, 1023, -MAX_MOVIMENTO, MAX_MOVIMENTO, 100);
```


#### Read and Movement Functions
The functions responsible for controlling both motors to move or not are `readAndMove()`, `horMov()`, and `verMov()`. 

In reality, these three functions differ in few aspects (ignoring the obvious difference between `horMov()` and `verMov()`, which differ in the orientation of the displacement) since, in practical terms, they accomplish the same task. However, the existence and invocation of each one of them depend on what needs to be tested.

The utility of `readAndMove()` is that it first calculates the current position of both motors to eventually move them. Meanwhile, `horMov()` and `verMov()` allow reading the current position and moving the top motor only if the movement of the base motor has already finished.

For the sake of practicality, we will break down the operation of `horMov()` for explanation.

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
              base_position += horizontalSteps;
              moveBase(horizontalSteps);
          }
       }
}
```
However, before concluding the breakdown of this function, we encounter the invocation of `moveBase()`, which indeed is responsible for the operation of the motor at the logical-mathematical level. The `moveBase()` function, along with other directly related functions, is found in the *StepperLib* library, where we can also find functions responsible for the top motor.

The function below (which is within the definition of the library) sends a single pulse to the control pin of the base motor. Each motor pulse produces a small angular movement. These pulses are essential for the motor to advance or reverse one step at a time. The duration of the pulse and the delay between pulses can be adjusted to control the speed and smoothness of the motor's movement.
```c
void stepBaseMotor() {
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, BASE_STEP_PIN);
        __delay_cycles(5000); 
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, BASE_STEP_PIN);
}
```
Finally, referring to `moveBase()`, it controls the direction of movement of the base motor and the number of steps it should take. Depending on the "steps" parameter, the motor will move forward or backward. The steps are generated using the `stepBaseMotor()` function in a for loop. This ensures that the base motor moves exactly the specified number of steps.

```c
void moveBase(int steps) {
    if (steps > 0) {
        // moveBaseForward
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, BASE_DIR_PIN);
    } else {
        // moveBaseBackward
        steps = 0 - steps;
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, BASE_DIR_PIN);
    }
    // Generate steps
    int i;
    for (i = 0; i < steps; i++) {
        stepBaseMotor();
        __delay_cycles(250); 
    }
}
```
Summarizing, these functions are essential for the operation of the stepper motors. They provide the precise control needed to move the solar panels in the correct direction and distance to maximize solar energy capture. Without proper control of the motor steps, solar tracking would not be possible.

#### Step restriction
*Post a photo of the Solar Tracker and explain why it would be harmful for the arm to move in any direction.*




## Team
| Members        | Mail |
|--|--|
| Andrea Pedrini | andrea.pedrini@studenti.unitn.it |
| Irina Voloshina | irina.voloshina@studenti.unitn.it |
| Riccardo Stiz | riccardo.stiz@studenti.unitn.it |
| Francheska Paz Contreras Delgado | fp.contrerasdelgado@studenti.unitn.it |
