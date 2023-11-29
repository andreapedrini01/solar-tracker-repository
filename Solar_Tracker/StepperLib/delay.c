#include "StepperLib/delay.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"

// Inizializza il timer
void initTimer() {
    // Configura Timer32 come timer di conteggio continuo a 48 MHz
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
                       TIMER32_PERIODIC_MODE);

    // Imposta il periodo del timer (conta fino a 48M per un secondo)
    Timer32_setCount(TIMER32_0_BASE, 48000000);
}

// Ritardo in millisecondi utilizzando il timer
void delayMilliseconds(uint32_t ms) {
    // Configura il timer A
    Timer_A_initUpModeParam initUpParam = {0};
    initUpParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initUpParam.timerPeriod = 32768 - 1; // Frequenza ACLK = 32768 Hz
    initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initUpParam.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initUpParam.timerClear = TIMER_A_DO_CLEAR;
    initUpParam.startTimer = false;

    Timer_A_initUpMode(TIMER_A0_BASE, &initUpParam);

    // Imposta il periodo del timer
    Timer_A_setCaptureCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0,
                                   32768 - 1);

    // Avvia il timer
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    // Attendi il tempo specificato
    while (ms > 0) {
        // Attendi che il timer raggiunga il contatore di overflow
        while (!(Timer_A_getInterruptStatus(TIMER_A0_BASE) & TIMER_A_CAPTURECOMPARE_REGISTER_0));

        // Pulisci l'interrupt del timer
        Timer_A_clearInterrupt(TIMER_A0_BASE);

        // Decrementa il contatore di millisecondi
        ms--;
    }

    // Ferma il timer
    Timer_A_stopTimer(TIMER_A0_BASE);
}
