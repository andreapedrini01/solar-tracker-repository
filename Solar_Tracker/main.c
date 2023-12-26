#include "msp.h"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include <string.h>
#include <StepperLib/base_stepper.h>
#include <stepperLib/top_stepper.h>

#define NUM_SENSORS 4
#define VALUE_CHANGE 70
#define LIGHT_THRESHOLD 300 // Adjust this threshold as needed
#define MAX_STEPS_X 100
#define MAX_STEPS_Y 100

#define MOVIMENTO 3000
#define MAX_MOVIMENTO 5000

static uint16_t resultsBuffer[NUM_SENSORS];
int horizontalSteps = 0;
int verticalSteps = 0;
int diff1 = 0;
int diff2 = 0;


/* Graphic library context */
//Graphics_Context g_sContext;

/*void text_case_0() {
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "--Voltage:",
                                AUTO_STRING_LENGTH, 64, 15, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Current:",
                                AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);
}

void text_case_1() {
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Voltage:",
                                AUTO_STRING_LENGTH, 64, 15, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "--Current:",
                                AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);
}*/

/*void _graphicsInit()
{
    // Initializes display
    Crystalfontz128x128_Init();

    // Set default screen orientation
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    // Initializes graphics context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    text_case_0();

}*/

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
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void readAndMove() {

   int i=0;

   /* ADC_MEM1 conversion completed */
  if(ADC_INT1)
       {
       /* Store ADC14 conversion results */
       resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
       resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
       resultsBuffer[2] = ADC14_getResult(ADC_MEM2);
       resultsBuffer[3] = ADC14_getResult(ADC_MEM3);

       /*printf("PR1: %5d\n", resultsBuffer[0]);
       printf("PR2: %5d\n", resultsBuffer[1]);
       printf("PR3: %5d\n", resultsBuffer[2]);
       printf("PR4: %5d\n\n", resultsBuffer[3]);*/
   }

   int avgIntensity = 0;

   for (i = 0; i < NUM_SENSORS; i++) {
       avgIntensity += resultsBuffer[i];
   }
   avgIntensity /= NUM_SENSORS;

   if (avgIntensity > LIGHT_THRESHOLD) {
       diff1 = resultsBuffer[2] - resultsBuffer[3];
       /* See if there's an actual change in the value */
       if (abs(diff1) >= VALUE_CHANGE) {
           horizontalSteps = map(diff1, 0, 16383, 0, MAX_STEPS_X);
       }

       diff2 = resultsBuffer[3] - resultsBuffer[2];
       /* See if there's an actual change in the value */
       if (abs(diff2) >= VALUE_CHANGE) {
           verticalSteps = map(diff2, 0, 16383, 0, MAX_STEPS_Y);
       }
       //verticalSteps = 0; //REMEMBER TO CHANGE

       // control if the motion has to be clockwise or anti-clockwise and send the impulses
       if (horizontalSteps != 0) {
           moveBase(horizontalSteps);
       }

       if (verticalSteps != 0) {
           moveTop(verticalSteps);
       }

       int maxSteps = 0;
       if(horizontalSteps > verticalSteps)
           maxSteps = horizontalSteps;
       else maxSteps = verticalSteps;
       for (i = 0; i < maxSteps; i++) {
           if(maxSteps - i < FINAL_STEPS) {
               puts("SLOWER DELAY");
               __delay_cycles(SLOWER_DELAY);
           }
           else {
               puts("FASTER DELAY");
               __delay_cycles(FASTER_DELAY);
           }
       }
   }
   puts("-------------");
   __delay_cycles(100);
}

/*
 * Main function
 */
void main(void)
{

    _hwInit();

    //moveTop(-500);

    int counter = 0;
    int i=0;

   while(i < 2) {
    if(counter + MOVIMENTO <= 5000) {
        counter += MOVIMENTO;
        moveTop(MOVIMENTO);
    } else {
        int diff = abs(counter - MAX_MOVIMENTO);
        counter += diff;
        moveTop(diff);
    }
    i++;
    __delay_cycles(2000000);
   }

   moveTop(-counter);

    while(1){

        //readAndMove();

        //moveTop(3000);
        //moveBase(10);
        //__delay_cycles(2000000);
        //moveTop(-3000);
        //moveBase(-10);
        //__delay_cycles(2000000);

   }
}
