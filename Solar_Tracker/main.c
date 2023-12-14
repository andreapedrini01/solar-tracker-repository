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
#define LIGHT_THRESHOLD 500 // Adjust this threshold as needed
#define HORIZONTAL_MIN_ANGLE 0
#define HORIZONTAL_MAX_ANGLE 180
#define VERTICAL_MIN_ANGLE 0
#define VERTICAL_MAX_ANGLE 180
#define MAX_STEPS_X 50
#define MAX_STEPS_Y 50

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

void _hwInit()
{
    // Halting WDT and disabling master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    //_graphicsInit();
    init_motors();
}

int map(int x, int in_min, int in_max, int out_min, int out_max)    //function useful in photoresistor algorithm
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void readAndMove() {

   int sensorValues[NUM_SENSORS];
   int i=0;

   for (i = 0; i < NUM_SENSORS; i++) {
       ADC14->CTL0 |= ADC14_CTL0_SC; // Start conversion
       while (!(ADC14->IFGR0 & BIT(i))); // Wait for conversion to complete
       sensorValues[i] = ADC14->MEM[i];
   }

   int avgIntensity = 0;

   for (i = 0; i < NUM_SENSORS; i++) {
       avgIntensity += sensorValues[i];
   }
   avgIntensity /= NUM_SENSORS;

   if (avgIntensity > LIGHT_THRESHOLD) {
       int horizontalSteps = map(sensorValues[1] - sensorValues[0], 0, 1023, 0, MAX_STEPS_X);
       int verticalSteps = map(sensorValues[3] - sensorValues[2], 0, 1023, 0, MAX_STEPS_Y);

       // control if the motion has to be clockwise or anti-clockwise and send the impulses
       if (horizontalSteps > 0) {
           moveBaseForward(horizontalSteps);
       } else {
           moveBaseBackward(0 - horizontalSteps);
           }

       if (verticalSteps > 0) {
           moveTopForward(verticalSteps);
       } else {
           moveTopBackward(0 - verticalSteps);
           }

   }

   __delay_cycles(100);
}

/*
 * Main function
 */
void main(void)
{

    _hwInit();

    while(1){

        moveTopForward(100);
        moveBaseBackward(100);
        __delay_cycles(2000000);
        moveTopBackward(100);
        moveBaseForward(100);
        __delay_cycles(2000000);

   }
}
