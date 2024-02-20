#include "msp.h"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <stdio.h>
#include <StepperLib/base_stepper.h>
#include <stepperLib/top_stepper.h>

#define NUM_SENSORS 4
#define VALUE_CHANGE 30
#define LIGHT_THRESHOLD 0 // Adjust this threshold as needed
#define MOVIMENTO 3000
#define MAX_MOVIMENTO 5000

static uint16_t resultsBuffer[NUM_SENSORS];

int boolFunction = 1;
int base_position = 0;
int top_position = 0;

void init_motors() {
    //base motor
    init_baseStepper();

    //top motor
    init_topStepper();
}

void _adcInit(){
    /* Configures Pin 6.0 and 4.4 as ADC input */
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

        /* Initializing ADC (ADCOSC/64/8) */
        ADC14_enableModule();
        ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

        /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM1 (A15, A9)  with repeat)
             * with internal 2.5v reference */
        ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM3, true);
        ADC14_configureConversionMemory(ADC_MEM0,
                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A3, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(ADC_MEM1,
                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A4, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(ADC_MEM2,
                        ADC_VREFPOS_AVCC_VREFNEG_VSS,
                        ADC_INPUT_A5, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(ADC_MEM3,
                                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                ADC_INPUT_A1, ADC_NONDIFFERENTIAL_INPUTS);

        /* Setting up the sample timer to automatically step through the sequence
         * convert.
         */
        ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

        /* Triggering the start of the sample */
        ADC14_enableConversion();
        ADC14_toggleConversionTrigger();
}

void _hwInit()
{
    // Halting WDT and disabling master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    //_graphicsInit();
    init_motors();
    _adcInit();
}

int map(int x, int in_min, int in_max, int out_min, int out_max)    //function useful in photoresistor algorithm
{
    long int top_part = (x - in_min) * (out_max - out_min);
    printf("top_part = %d\n", top_part);
    int bottom_part = in_max - in_min;
    return  (top_part / bottom_part) + out_min;
}

int limitSteps(int counter, int movement) {
    int steps = movement;
    // Upper limit check
    if (counter+movement > MAX_MOVIMENTO) {
        steps = movement - MAX_MOVIMENTO - MAX_MOVIMENTO;
    }
    // Lower limit check
    if (counter+movement < 0-MAX_MOVIMENTO) {
        steps = movement + MAX_MOVIMENTO + MAX_MOVIMENTO;
    }
    return steps;
}

int scaleReading(reading) {
    return map(reading, 0, 16383, 0, 1023);
}

void readAndMove() {

   int horizontalSteps = 0;
   int verticalSteps = 0;
   int diff1 = 0;
   int diff1_1 = 0;
   int diff2 = 0;
   int diff2_2 = 0;
   int i=0;

  /* Store ADC14 conversion results */
       resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
       resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
       resultsBuffer[2] = ADC14_getResult(ADC_MEM2);
       resultsBuffer[3] = ADC14_getResult(ADC_MEM3);

         printf("PR0: %5d\n", resultsBuffer[0]);
         printf("PR1: %5d\n", resultsBuffer[1]);
         printf("PR2: %5d\n", resultsBuffer[2]);
         printf("PR3: %5d\n\n", resultsBuffer[3]);

   int avgIntensity = 0;

   for (i = 0; i < NUM_SENSORS; i++) {
       avgIntensity += resultsBuffer[i];
   }
   avgIntensity /= NUM_SENSORS;

   if (avgIntensity > LIGHT_THRESHOLD) {
       diff1 = resultsBuffer[0] - resultsBuffer[1];
       diff1_1 = resultsBuffer[3] - resultsBuffer[2];
       printf("diff1 = %d\n", diff1);
       printf("diff1_1 = %d\n", diff1_1);
       /* See if there's an actual change in the value */
       if (abs(diff1) >= VALUE_CHANGE) {
           horizontalSteps = map(diff1, -16383, 16383, -10000, 10000);
       } else if (abs(diff1_1) >= VALUE_CHANGE) {
           horizontalSteps = map(diff1_1, -16383, 16383, -10000, 10000);
       }
       printf("horizontalSteps before limiting = %d\n", horizontalSteps);

       diff2 = resultsBuffer[0] - resultsBuffer[3];
       diff2_2 = resultsBuffer[1] - resultsBuffer[2];
       /* See if there's an actual change in the value */
       if (abs(diff2) >= VALUE_CHANGE)
           verticalSteps = map(diff2, -16383, 16383, -10000, 10000);
       else if (abs(diff2_2) >= VALUE_CHANGE)
           verticalSteps = map(diff2_2, -16383, 16383, -10000, 10000);
       // control if the motion has to be clockwise or anti-clockwise and send the impulses
       if (horizontalSteps != 0) {
          horizontalSteps = limitSteps(base_position,horizontalSteps);
          printf("horizontalSteps after limiting = %d\n", horizontalSteps);
          base_position += horizontalSteps;
          moveBase(horizontalSteps);
      }

       printf("\n");

       if (verticalSteps != 0) {
           verticalSteps = limitSteps(top_position, verticalSteps);
           top_position += verticalSteps;
           moveTop(verticalSteps);
       }
   }
   __delay_cycles(100);
}

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

       int avgIntensity = 0;
       int i=0;
       for (i = 0; i < NUM_SENSORS; i++) {
           avgIntensity += resultsBuffer[i];
       }
       avgIntensity /= NUM_SENSORS;

       if (avgIntensity > LIGHT_THRESHOLD) {
           diff1 = resultsBuffer[0] - resultsBuffer[1];
           diff1_1 = resultsBuffer[3] - resultsBuffer[2];
           printf("diff1 = %d\n", diff1);
           printf("diff1_1 = %d\n", diff1_1);
           /* See if there's an actual change in the value */
           if (abs(diff1) >= VALUE_CHANGE) {
                      horizontalSteps = map(diff1, -16383, 16383, -10000, 10000);
           } else if (abs(diff1_1) >= VALUE_CHANGE)
                      horizontalSteps = map(diff1_1, -16383, 16383, -10000, 10000);

           // control if the motion has to be clockwise or anti-clockwise and send the impulses
           if (horizontalSteps != 0) {
              horizontalSteps = limitSteps(base_position,horizontalSteps);
              printf("horizontalSteps after limiting = %d\n", horizontalSteps);
              base_position += horizontalSteps;
              moveBase(horizontalSteps);
          }
       }
}

void verMov() {
   int verticalSteps = 0;
   int diff2 = 0;
   int diff2_2 = 0;

      /* Store ADC14 conversion results */
           resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
           resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
           resultsBuffer[2] = ADC14_getResult(ADC_MEM2);
           resultsBuffer[3] = ADC14_getResult(ADC_MEM3);

           printf("PR0: %5d\n", resultsBuffer[0]);
           printf("PR1: %5d\n", resultsBuffer[1]);
           printf("PR2: %5d\n", resultsBuffer[2]);
           printf("PR3: %5d\n\n", resultsBuffer[3]);

       int avgIntensity = 0;
       int i=0;
       for (i = 0; i < NUM_SENSORS; i++) {
           avgIntensity += resultsBuffer[i];
       }
       avgIntensity /= NUM_SENSORS;

       diff2 = resultsBuffer[0] - resultsBuffer[3];
       diff2_2 = resultsBuffer[1] - resultsBuffer[2];
       /* See if there's an actual change in the value */
       if (abs(diff2) >= VALUE_CHANGE)
                  verticalSteps = map(diff2, -16383, 16383, -10000, 10000);
              else if (abs(diff2_2) >= VALUE_CHANGE)
                  verticalSteps = map(diff2_2, -16383, 16383, -10000, 10000);

       if (verticalSteps != 0) {
          verticalSteps = limitSteps(top_position, verticalSteps);
          top_position += verticalSteps;
          moveTop(verticalSteps);
       }
}

/*
 * Main function
 */
void main(void) {

    _hwInit();

    /*while(1) {
            moveTop(1);
        }*/

    if (boolFunction == 1) {
        while(1){
                // ADC_MEM1 conversion completed
                if(!ADC_INT1)
                   continue;

                readAndMove();

           }
    } else {
        int fullMovementCompleted = 1;
        while(1){
                // ADC_MEM1 conversion completed
                if(!ADC_INT1)
                   continue;

                if (fullMovementCompleted == 1) {
                    // move horizontal
                    horMov();
                    fullMovementCompleted = 0;
                } else {
                    //move vertical
                    verMov();
                    fullMovementCompleted = 1;
                }

           }
    }
}
