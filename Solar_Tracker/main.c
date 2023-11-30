#include "msp.h"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include <string.h>
#include <StepperLib/base_stepper.h>

/* Graphic library context */
Graphics_Context g_sContext;

void text_case_0() {
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
}

void init_motors() {
    //base motor
    init_baseStepper();
}

void _graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    text_case_0();

}

void _hwInit()
{
    // Halting WDT and disabling master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    // Set the core voltage level to VCORE1
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    // Set 2 flash wait states for Flash bank 0 and 1
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    _graphicsInit();
    init_motors();
}



/*
 * Main function
 */
void main(void)
{

    _hwInit();

    moveBaseForward(50);

    moveBaseForward(50);

    //unsigned int index = 0;

    while(1){
        //PCM_gotoLPM0();


        //__delay_cycles(2000000);

   }
}
