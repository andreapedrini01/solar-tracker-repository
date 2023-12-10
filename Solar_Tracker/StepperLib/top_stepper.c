#include <StepperLib/top_stepper.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"

void init_topStepper() {
    // Inizializza la port e i pin
    MAP_WDT_A_holdTimer();

    // Configura i pin come output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, STEP_PIN | DIR_PIN | ENABLE_PIN);

    // Imposta i pin a livello basso iniziale
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, STEP_PIN | DIR_PIN | ENABLE_PIN);
}

void stepTopMotor() {
    // Invia un impulso STEP
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, STEP_PIN);
        __delay_cycles(250); // Aggiungi un piccolo ritardo
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, STEP_PIN);
}

void moveTopForward(int steps) {
    // Imposta la direzione in avanti
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, DIR_PIN);

        // Genera i passi
        int i;
        for (i = 0; i < steps; i++) {
            stepTopMotor();
            __delay_cycles(5000); // Aggiungi un ritardo tra i passi
        }
}

void moveTopBackward(int steps) {
    // Imposta la direzione all'indietro
       MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, DIR_PIN);

       // Genera i passi
       int i;
       for (i = 0; i < steps; i++) {
               stepTopMotor();
                   __delay_cycles(5000); // Aggiungi un ritardo tra i passi
       }
}

void lock_topEnable() {
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, ENABLE_PIN);
}

void unlock_topEnable() {
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, ENABLE_PIN);
}
