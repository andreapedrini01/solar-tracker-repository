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

/* ADC results buffer */
static uint16_t resultsBuffer[2];

/* Y axis threshold for joystick pulled down*/
const unsigned int Y_THRESHOLD = 7000;

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

void _adcInit(){
    // Configures Pin 6.0 and 4.4 as ADC input
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

        // Initializing ADC (ADCOSC/64/8)
        ADC14_enableModule();
        ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

        // Configuring ADC Memory (ADC_MEM0 - ADC_MEM1 (A15, A9)  with repeat)
        // with internal 2.5v reference
        ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);
        ADC14_configureConversionMemory(ADC_MEM0,
                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(ADC_MEM1,
                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

        // Enabling the interrupt when a conversion on channel 1 (end of sequence)
        //  is complete and enabling conversions
        ADC14_enableInterrupt(ADC_INT1);

        // Enabling Interrupts
        Interrupt_enableInterrupt(INT_ADC14);
        Interrupt_enableMaster();

        // Setting up the sample timer to automatically step through the sequence
        //  convert.
        ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

        // Triggering the start of the sample
        ADC14_enableConversion();
        ADC14_toggleConversionTrigger();
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

    // Initializes Clock System
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    _graphicsInit();
    _adcInit();
    init_motors();
}



/*
 * Main function
 */
void main(void)
{

    _hwInit();

    unsigned int index = 0;

    while(1){
        //PCM_gotoLPM0();

                moveBaseForward(50);
                           __delay_cycles(2000000);
                           moveBaseBackward(50);
                           __delay_cycles(2000000);

        if(resultsBuffer[1] <= Y_THRESHOLD){
            Graphics_clearDisplay(&g_sContext);
            index++;
            if(index >= 2)
                index = 0;
            switch(index){
                    case 0: text_case_0();
                            break;
                    case 1: text_case_1();
                            break;
                    default:Graphics_drawStringCentered(&g_sContext, (int8_t *) "ERROR!!!",
                                                        AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);
                }
            while(resultsBuffer[1] <= Y_THRESHOLD){
                //This while is required to block the scrolling loop that causes the LCD to flash;
            }
        }


   }
}

void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    /* ADC_MEM1 conversion completed */
    if(status & ADC_INT1)
    {
        /* Store ADC14 conversion results */
        resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
        resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
        printf("X: %d, Y: %d\n", resultsBuffer[0], resultsBuffer[1]);

        /*char string[10];
        sprintf(string, "X: %5d", resultsBuffer[0]);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        8,
                                        64,
                                        64,
                                        OPAQUE_TEXT);

        sprintf(string, "Y: %5d", resultsBuffer[1]);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        8,
                                        64,
                                        84,
                                        OPAQUE_TEXT);*/

        /* Determine if JoyStick button is pressed */
        /*int buttonPressed = 0;
        if (!(P4IN & GPIO_PIN1))
            buttonPressed = 1;*/

        /*sprintf(string, "Button: %d", buttonPressed);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        AUTO_STRING_LENGTH,
                                        64,
                                        104,
                                        OPAQUE_TEXT);*/
    }
}
